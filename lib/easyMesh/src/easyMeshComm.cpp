//
//  easyMeshComm.cpp
//
//
//  Created by Bill Gray on 7/26/16.
//
//

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SimpleList.h>

#include "easyMesh.h"

extern easyMesh *staticThis;

// communications functions
//***********************************************************************
bool ICACHE_FLASH_ATTR easyMesh::sendMessage(meshConnectionType *conn,
                                             uint32_t destId,
                                             meshPackageType type,
                                             String &msg) {
  debugMsg(COMMUNICATION,
           "sendMessage(conn): conn-chipId=%d destId=%d type=%d msg=%s\n",
           conn->chipId, destId, (uint8_t)type, msg.c_str());

  String package = buildMeshPackage(destId, type, msg);

  return sendPackage(conn, package);
}

//***********************************************************************
bool ICACHE_FLASH_ATTR easyMesh::sendMessage(uint32_t destId,
                                             meshPackageType type,
                                             String &msg) {
  debugMsg(COMMUNICATION, "In sendMessage(destId): destId=%d type=%d, msg=%s\n",
           destId, type, msg.c_str());

  meshConnectionType *conn = findConnection(destId);
  if (conn != NULL) {
    return sendMessage(conn, destId, type, msg);
  } else {
    debugMsg(ERROR,
             "In sendMessage(destId): findConnection( destId ) failed\n");
    return false;
  }
}

//***********************************************************************
bool ICACHE_FLASH_ATTR easyMesh::broadcastMessage(uint32_t from,
                                                  meshPackageType type,
                                                  String &msg,
                                                  meshConnectionType *exclude) {

  // send a message to every node on the mesh

  if (exclude != NULL)
    debugMsg(COMMUNICATION,
             "broadcastMessage(): from=%d type=%d, msg=%s exclude=%d\n", from,
             type, msg.c_str(), exclude->chipId);
  else
    debugMsg(COMMUNICATION,
             "broadcastMessage(): from=%d type=%d, msg=%s exclude=NULL\n", from,
             type, msg.c_str());

  SimpleList<meshConnectionType>::iterator connection = _connections.begin();
  while (connection != _connections.end()) {
    if (connection != exclude) {
      sendMessage(connection, connection->chipId, type, msg);
    }
    connection++;
  }
  return true; // hmmm... ought to be smarter than this!
}

//***********************************************************************
bool ICACHE_FLASH_ATTR easyMesh::sendPackage(meshConnectionType *connection,
                                             String &package) {
  debugMsg(COMMUNICATION, "Sending to %d-->%s<--\n", connection->chipId,
           package.c_str());

  if (package.length() > 1400)
    debugMsg(ERROR, "sendPackage(): err package too long length=%d\n",
             package.length());

  if (connection->sendReady == true) {
    sint8 errCode = espconn_send(connection->esp_conn, (uint8 *)package.c_str(),
                                 package.length());
    connection->sendReady = false;

    if (errCode == 0) {
      return true;
    } else {
      debugMsg(ERROR, "sendPackage(): espconn_send Failed err=%d\n", errCode);
      return false;
    }
  } else {
    connection->sendQueue.push_back(package);
  }
}

//***********************************************************************
String ICACHE_FLASH_ATTR easyMesh::buildMeshPackage(uint32_t destId,
                                                    meshPackageType type,
                                                    String &msg) {
  debugMsg(GENERAL, "In buildMeshPackage(): msg=%s\n", msg.c_str());

  DynamicJsonBuffer jsonBuffer(JSON_BUFSIZE);
  JsonObject &root = jsonBuffer.createObject();
  root["dest"] = destId;
  root["from"] = _chipId;
  root["type"] = (uint8_t)type;

  switch (type) {
  case NODE_SYNC_REQUEST:
  case NODE_SYNC_REPLY: {
    JsonArray &subs = jsonBuffer.parseArray(msg);
    if (!subs.success()) {
      debugMsg(
          GENERAL,
          "buildMeshPackage(): subs = jsonBuffer.parseArray( msg ) failed!");
    }
    root["subs"] = subs;
    break;
  }
  case TIME_SYNC:
    root["msg"] = jsonBuffer.parseObject(msg);
    break;
  default:
    root["msg"] = msg;
  }

  String ret;
  root.printTo(ret);
  return ret;
}
( remoteChipId ) != NULL) ) {
  // this is the first NODE_SYNC_REQUEST from a station
  // is we are already connected drop this connection
  debugMsg(SYNC, "handleNodeSync(): Already connected to node %d.  Dropping\n",
           conn->chipId);
  closeConnection(conn);
  return;
}

if (conn->chipId != remoteChipId) {
  debugMsg(SYNC, "handleNodeSync(): conn->chipId updated from %d to %d\n",
           conn->chipId, remoteChipId);
  conn->chipId = remoteChipId;
}

// check to see if subs have changed.
String inComingSubs = root["subs"];
if (!conn->subConnections.equals(inComingSubs)) { // change in the network
  reSyncAllSubConnections = true;
  conn->subConnections = inComingSubs;
}

switch (type) {
case NODE_SYNC_REQUEST: {
  debugMsg(
      SYNC,
      "handleNodeSync(): valid NODE_SYNC_REQUEST %d sending NODE_SYNC_REPLY\n",
      conn->chipId);
  String myOtherSubConnections = subConnectionJson(conn);
  sendMessage(conn, _chipId, NODE_SYNC_REPLY, myOtherSubConnections);
  break;
}
case NODE_SYNC_REPLY:
  debugMsg(SYNC, "handleNodeSync(): valid NODE_SYNC_REPLY from %d\n",
           conn->chipId);
  conn->nodeSyncRequest = 0; // reset nodeSyncRequest Timer  ????
  if (conn->lastTimeSync == 0)
    startTimeSync(conn);
  break;
default:
  debugMsg(ERROR, "handleNodeSync(): weird type? %d\n", type);
}

if (reSyncAllSubConnections == true) {
  SimpleList<meshConnectionType>::iterator connection = _connections.begin();
  while (connection != _connections.end()) {
    connection->nodeSyncStatus = NEEDED;
    connection++;
  }
}

conn->nodeSyncStatus = COMPLETE; // mark this connection nodeSync'd
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::startTimeSync(meshConnectionType *conn) {
  debugMsg(SYNC, "startTimeSync(): with %d\n", conn->chipId);

  if (conn->time.num > TIME_SYNC_CYCLES) {
    debugMsg(
        ERROR,
        "startTimeSync(): Error timeSync.num not reset conn->time.num=%d\n",
        conn->time.num);
  }

  conn->time.num = 0;

  conn->time.adopt = adoptionCalc(conn); // do I adopt the estblished time?
  //   debugMsg( GENERAL, "startTimeSync(): remoteSubCount=%d adopt=%d\n",
  //   remoteSubCount, conn->time.adopt);

  String timeStamp = conn->time.buildTimeStamp();
  staticThis->sendMessage(conn, _chipId, TIME_SYNC, timeStamp);

  conn->timeSyncStatus = IN_PROGRESS;
}

//***********************************************************************
bool ICACHE_FLASH_ATTR easyMesh::adoptionCalc(meshConnectionType *conn) {
  // make the adoption calulation.  Figure out how many nodes I am connected to
  // exclusive of this connection.

  uint16_t mySubCount = connectionCount(conn); // exclude this connection.
  uint16_t remoteSubCount = jsonSubConnCount(conn->subConnections);

  bool ret = (mySubCount > remoteSubCount) ? false : true;

  debugMsg(GENERAL,
           "adoptionCalc(): mySubCount=%d remoteSubCount=%d ret = %d\n",
           mySubCount, remoteSubCount, ret);

  return ret;
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::handleTimeSync(meshConnectionType *conn,
                                                JsonObject &root) {

  String timeStamp = root["msg"];
  debugMsg(SYNC, "handleTimeSync(): with %d in timestamp=%s\n", conn->chipId,
           timeStamp.c_str());

  conn->time.processTimeStamp(
      timeStamp); // varifies timeStamp and updates it with a new one.

  debugMsg(SYNC, "handleTimeSync(): with %d out timestamp=%s\n", conn->chipId,
           timeStamp.c_str());

  if (conn->time.num < TIME_SYNC_CYCLES) {
    staticThis->sendMessage(conn, _chipId, TIME_SYNC, timeStamp);
  }

  uint8_t odd = conn->time.num % 2;

  if ((conn->time.num + odd) >= TIME_SYNC_CYCLES) { // timeSync completed
    if (conn->time.adopt) {
      conn->time.calcAdjustment(odd);

      // flag all connections for re-timeSync
      SimpleList<meshConnectionType>::iterator connection =
          _connections.begin();
      while (connection != _connections.end()) {
        if (connection != conn) { // exclude this connection
          connection->timeSyncStatus = NEEDED;
        }
        connection++;
      }
    }
    conn->lastTimeSync = getNodeTime();
    conn->timeSyncStatus = COMPLETE;
  }
}
nnCount() : out of memory2\n ");
}

str = obj.get<String>("subs");
count += (1 + jsonSubConnCount(str));
}

debugMsg(CONNECTION, "jsonSubConnCount(): leaving count=%d\n", count);

return count;
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::meshConnectedCb(void *arg) {
  staticThis->debugMsg(CONNECTION,
                       "meshConnectedCb(): new meshConnection !!!\n");
  meshConnectionType newConn;
  newConn.esp_conn = (espconn *)arg;
  espconn_set_opt(
      newConn.esp_conn,
      ESPCONN_NODELAY); // removes nagle, low latency, but soaks up bandwidth
  newConn.lastRecieved = staticThis->getNodeTime();

  espconn_regist_recvcb(newConn.esp_conn, meshRecvCb);
  espconn_regist_sentcb(newConn.esp_conn, meshSentCb);
  espconn_regist_reconcb(newConn.esp_conn, meshReconCb);
  espconn_regist_disconcb(newConn.esp_conn, meshDisconCb);

  staticThis->_connections.push_back(newConn);

  if (newConn.esp_conn->proto.tcp->local_port !=
      staticThis->_meshPort) { // we are the station, start nodeSync
    staticThis->debugMsg(CONNECTION,
                         "meshConnectedCb(): we are STA, start nodeSync\n");
    staticThis->startNodeSync(staticThis->_connections.end() - 1);
    newConn.timeSyncStatus = NEEDED;
  } else
    staticThis->debugMsg(CONNECTION, "meshConnectedCb(): we are AP\n");

  staticThis->debugMsg(GENERAL, "meshConnectedCb(): leaving\n");
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::meshRecvCb(void *arg, char *data,
                                            unsigned short length) {
  meshConnectionType *receiveConn = staticThis->findConnection((espconn *)arg);

  staticThis->debugMsg(COMMUNICATION, "meshRecvCb(): data=%s fromId=%d\n", data,
                       receiveConn->chipId);

  if (receiveConn == NULL) {
    staticThis->debugMsg(
        ERROR, "meshRecvCb(): recieved from unknown connection 0x%x ->%s<-\n",
        arg, data);
    staticThis->debugMsg(ERROR, "dropping this msg... see if we recover?\n");
    return;
  }

  DynamicJsonBuffer jsonBuffer(JSON_BUFSIZE);
  JsonObject &root = jsonBuffer.parseObject(data);
  if (!root.success()) { // Test if parsing succeeded.
    staticThis->debugMsg(
        ERROR, "meshRecvCb: parseObject() failed. data=%s<--\n", data);
    return;
  }

  staticThis->debugMsg(GENERAL, "Recvd from %d-->%s<--\n", receiveConn->chipId,
                       data);

  String msg = root["msg"];

  switch ((meshPackageType)(int)root["type"]) {
  case NODE_SYNC_REQUEST:
  case NODE_SYNC_REPLY:
    staticThis->handleNodeSync(receiveConn, root);
    break;

  case TIME_SYNC:
    staticThis->handleTimeSync(receiveConn, root);
    break;

  case SINGLE:
    if ((uint32_t)root["dest"] == staticThis->getChipId()) { // msg for us!
      receivedCallback((uint32_t)root["from"], msg);
    } else { // pass it along
      // staticThis->sendMessage( (uint32_t)root["dest"],
      // (uint32_t)root["from"], SINGLE, msg );  //this is ineffiecnt
      String tempStr(data);
      staticThis->sendPackage(
          staticThis->findConnection((uint32_t)root["dest"]), tempStr);
    }
    break;

  case BROADCAST:
    staticThis->broadcastMessage((uint32_t)root["from"], BROADCAST, msg,
                                 receiveConn);
    receivedCallback((uint32_t)root["from"], msg);
    break;

  default:
    staticThis->debugMsg(ERROR,
                         "meshRecvCb(): unexpected json, root[\"type\"]=%d",
                         (int)root["type"]);
    return;
  }

  // record that we've gotten a valid package
  receiveConn->lastRecieved = staticThis->getNodeTime();
  return;
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::meshSentCb(void *arg) {
  staticThis->debugMsg(GENERAL, "meshSentCb():\n"); // data sent successfully
  espconn *conn = (espconn *)arg;
  meshConnectionType *meshConnection = staticThis->findConnection(conn);

  if (meshConnection == NULL) {
    staticThis->debugMsg(ERROR, "meshSentCb(): err did not find "
                                "meshConnection? Likely it was dropped for "
                                "some reason\n");
    return;
  }

  if (!meshConnection->sendQueue.empty()) {
    String package = *meshConnection->sendQueue.begin();
    meshConnection->sendQueue.pop_front();
    sint8 errCode = espconn_send(meshConnection->esp_conn,
                                 (uint8 *)package.c_str(), package.length());
    // connection->sendReady = false;

    if (errCode != 0) {
      staticThis->debugMsg(ERROR, "meshSentCb(): espconn_send Failed err=%d\n",
                           errCode);
    }
  } else {
    meshConnection->sendReady = true;
  }
}
//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::meshDisconCb(void *arg) {
  struct espconn *disConn = (espconn *)arg;

  staticThis->debugMsg(CONNECTION, "meshDisconCb(): ");

  // test to see if this connection was on the STATION interface by checking the
  // local port
  if (disConn->proto.tcp->local_port == staticThis->_meshPort) {
    staticThis->debugMsg(
        CONNECTION, "AP connection.  No new action needed. local_port=%d\n",
        disConn->proto.tcp->local_port);
  } else {
    staticThis->debugMsg(CONNECTION,
                         "Station Connection! Find new node. local_port=%d\n",
                         disConn->proto.tcp->local_port);
    // should start up automatically when station_status changes to IDLE
    wifi_station_disconnect();
  }

  return;
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::meshReconCb(void *arg, sint8 err) {
  staticThis->debugMsg(ERROR, "In meshReconCb(): err=%d\n", err);
}

//***********************************************************************
void ICACHE_FLASH_ATTR easyMesh::wifiEventCb(System_Event_t *event) {
  switch (event->event) {
  case EVENT_STAMODE_CONNECTED:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_STAMODE_CONNECTED ssid=%s\n",
                         (char *)event->event_info.connected.ssid);
    break;
  case EVENT_STAMODE_DISCONNECTED:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_STAMODE_DISCONNECTED\n");
    staticThis->connectToBestAP();
    break;
  case EVENT_STAMODE_AUTHMODE_CHANGE:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_STAMODE_AUTHMODE_CHANGE\n");
    break;
  case EVENT_STAMODE_GOT_IP:
    staticThis->debugMsg(CONNECTION, "wifiEventCb(): EVENT_STAMODE_GOT_IP\n");
    staticThis->tcpConnect();
    break;

  case EVENT_SOFTAPMODE_STACONNECTED:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_SOFTAPMODE_STACONNECTED\n");
    break;

  case EVENT_SOFTAPMODE_STADISCONNECTED:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_SOFTAPMODE_STADISCONNECTED\n");
    break;
  case EVENT_STAMODE_DHCP_TIMEOUT:
    staticThis->debugMsg(CONNECTION,
                         "wifiEventCb(): EVENT_STAMODE_DHCP_TIMEOUT\n");
    break;
  case EVENT_SOFTAPMODE_PROBEREQRECVED:
    // debugMsg( GENERAL, "Event: EVENT_SOFTAPMODE_PROBEREQRECVED\n");  // dont
    // need to know about every probe request
    break;
  default:
    staticThis->debugMsg(ERROR, "Unexpected WiFi event: %d\n", event->event);
    break;
  }
}

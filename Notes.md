# Notes
This is kind of my project diary. I will keep thoughts and ideas in here as well as in my notebook. If you are reading this, please don't laugh on how I think to myself. I just write whatever it comes to my mind. *Have Fun*
## Server
- One of the problems that I will face for sure, is that i don't know how to keep the devices in "ImAlive" state in the server. These devices don't have a clock that I can check, and without a proper server to check the time when the Request are done this is not gonna work. Having them synchronized when the device starts it is a good idea, but yes, you always need them to be connected to the internet.
- Once the time thing is done, I could start working on the real client.

## Wifi
- So, as far as I read you can only handle 5 device once the ESP8266 is in AP mode. This limitation could make that the design that I was thinking about connecting all of them to one is not viable.
- Connecting all the devices to the same home wifi could be the same idea, but I'll have to think what is the best option in terms of the Midi interface that I have to build. If each device acts like a midi controller, this will be way more complicated to handle.
- Connecting one to another, would that work? The second one connects to the network of the first one and builds its own network to share with the third one. It will also depends on how much devices I will be connecting.
- In terms of WiFi configuration, another idea could be to just configure the password in one of them, that password will be sent to the firebase api, and the first one will create a network in order to make the others connect.

## Midi
- About midi there are so many options. I could work with the devices as Inputs to the Web Midi Api that will act realtime on the Client. This could open lots of possibilities to the project and make it even better than just having to set a fucking local network to control everything. IoT, please.

## Accelerometer
- No news on this. The addresses of the accelerometers in I2C don't appear, will try to see how other protocols work and if they are connected to other thing.

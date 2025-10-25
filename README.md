

# HA-Control-Sphero-R2D2
Control your Sphero R2D2 with an ESP32 and call his animations from Home Assistant!
With this arduino code you can control the animations of a Sphero R2D2 droid toy. 
You cannot move him around manually as it's not something I had a need for and it wouldn't make much sense within home assistant anyway.
It will also report battery status to home assistant if you decide to leave him unplugged.

# How useful is it?
Honestly? It's really for fun. I have it in a simple automation where when I switch on my Gaming-PC it'll do a little happy dance, and when I turn the PC off, it'll be sad.

You could also use it if you have a smart doorbell to react to people ringing at your door. You can call it to be scared when the lights turn off...imagination is your limit!

# Credit where credit is due
This integration has been made possible with various resources on the internet of others that have already reverse engineered the sphero droids, namely R2D2.
Links to those resources can be found below

# Disclaimer
The arduino code has been created with the help of Claude. While I'm a system administrator by trade, I don't have the time to learn arduino code more than the basics. Within about 2 days of debugging and testing with Claude's help, the integration has been made possible.

# What you need:
- An ESP32 with a bluetooth and wifi chip like the NodeMCU ESP32-32S for instance.
- Home Assistant with the REST integration

# How it works:
- The ESP32 will connect to R2D2 through Bluetooth and wake him up
- The ESP32 will connect to your Wifi and provide a simple API where you can call the animations on R2D2

# A little action video

[![Watch the video](https://raw.githubusercontent.com/h311m4n000/HA-Control-Sphero-R2D2/main/r2d2.jpeg)](https://raw.githubusercontent.com/h311m4n000/HA-Control-Sphero-R2D2/main/r2d2.mp4)

# API Commands
Once the sketch has been uploaded to your ESP32, you can call these actions from your web browser (or use curl):
### Wake R2D2
**You must call this before you can call animations to wake your droid.**

    http://esp32_ip/wake
### Put R2D2 to sleep
    http://esp32_ip/sleep
 ### Call an R2D2 animation
    http://esp32_ip/anim?n=[0-56]
Were n is a number between 0 and 56
### Put R2D2 into tripod mode
    http://esp32_ip/stance?type=1
### Put R2D2 into bipod mode
    http://esp32_ip/stande?type=2
### Get the battery charge level
    http://esp32_ip/battery

# Home Assistant integration
I have not yet turned this into a full integration you can add under custom_components. 

## Configuration.yaml

Add the content of the configuration.yaml in this repository to your own. Change the ip address for the one of your ESP32.

## Scripts.yaml
These are all the actions (previously called services) you can call to simplify using the animations in your automations. Add the content of this file to your scripts.yaml and make sure your configuration.yaml includes this file.

### Links
- list of all the animations
[sphero-r2d2-animations](https://gist.github.com/ivesdebruycker/a31fcfd24d0cb554bf715a9d82c8be95)

- Reverse engineering of R2D2 by Andrea Stagi with node.js
[Reverse Engineering Sphero R2D2 with JavaScript - DEV Community](https://dev.to/astagi/reverse-engineering-sphero-r2d2-with-javascript-16ip)

- Andre Stagi's repo (I used index.js and made Claude convert it for arduino)
[astagi/freer2: ✨BLE reverse engineering with Star Wars R2D2 Sphero Droid](https://github.com/astagi/freer2)

- Emwire control of R2D2
[EMWiRES](http://www.emwires.com/ESP32USB/index.html#ESP02)

- Synack python script to control R2D2
[synack.net/~bbraun/spherodroid/r2.py](http://www.synack.net/~bbraun/spherodroid/r2.py)

- Synack article on interfacing with R2D2
[Scripting Sphero's Star Wars Droids](http://www.synack.net/~bbraun/spherodroid/)

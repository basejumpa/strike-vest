# Strike Vest

## Current scope and behavor

StrikeVest is available as a WiFi hotspot with the same name.
You can join the WiFi network w/o the need to enter a password.
StrikeVest allows up to 4 people being logged in.

Every WiFi client which connects gets automatically an IP address.

Every WiFi client is able to communicate via IP to the StrikeVest.

The first WiFi client which connects to StrikeVest it gets the following
network parameters.

- IP Address: 192.168.4.2
- Subnet Mask: 255.255.255.0
- Router 192.168.4.1
- Configure IP: Automatic
- Configure DNS: Automatic
- Configure Proxy: Off

From the WiFi client a ping to the IP address of the StrikeVest gets replies.

    ping 192.168.4.1

Browsing to http://192.168.4.1 does not work on the WiFi client.


## Expected behavior

StrikeVest is available as a WiFi hotspot with the same name.
You can join the WiFi network w/o the need to enter a password.
StrikeVest allows up to 4 people being logged in.

Every WiFi client which connects gets automatically an IP address.

Every WiFi client is able to communicate via IP to the StrikeVest.

From the WiFi client a ping to the IP address of the StrikeVest gets replies.

Browsing to the hardcoded IP address of StrikeVest works and the user sees "Welcome on the StrikeVest".

## Comparison between current against expected behaviors

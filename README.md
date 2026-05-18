# Goblin Phone Tester

Arduino thing that checks if I take my phone too often.

## used

- Arduino Uno
- obstacle avoidance sensor
- 1 digit 7 segment display
- red LED
- green LED
- buzzer
- some resistors

## Wiring

Obstacle sensor:

- VCC to 5V
- GND to GND
- OUT to D2

Other parts:

- red LED to D5
- green LED to D6
- buzzer to D9

7 segment display:

- A to D3
- B to D4
- C to D7
- D to D8
- E to D10
- F to D11
- G to D12

The common pin goes to GND if it is common cathode.  
If it is common anode, connect it to 5V and change this in the code:

```cpp
const bool commonAnode=true;

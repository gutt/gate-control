STEROWANIE:
ltv547
output -> transoptor (10k to 3.3V on arduino side PIN 4) 2k on PIN 1 output 5v

-> satel
1. output1 -> transoptor 0 / 1 (brama otwarta / brama zamknięta) output 12v
2. output2 -> transoptor 0 / 1 (stan wysoki - brama się otwiera) output 12v
3. output3 -> transoptor 0 / 1 (stan wysoki - brama się zamyka) output 12v

4. input1 -> up 
5. input2 -> down 

 mosquitto_sub -h 192.168.1.8 -p 1883 -u "darek" -P "wospamdK6" -t "homeassistant/#" -d

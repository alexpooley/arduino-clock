#define SERIAL_BAUD 115200

/*******************************************

	In this code we have defined ~ and | as 
	special symbols, so we can filter out
	serial data in between ~ and |
	and then process the commands like that.

	More of a proof of concept, but if you
	have a better example code, feel free
	to let us know at github@jaycar.com.au

*********************************************/
String recv = "";
bool data_in = false;

  
void setup() {
	// put your setup code here, to run once:

	Serial.begin(SERIAL_BAUD); //same as ESP baud

	Serial.println("Arduino Started, waiting for ~OK|");


	bool started = false;

	while(!started){						//wait until we've started

		while(Serial.available()){			//while / if serial data
			char c = Serial.read();			//read it

			if(c == '~'){					//start recv on ~
				data_in = true;
				recv = "";
				continue;					//read again

			} else if (data_in && c == '|'){	//finish recv on  |
				data_in = false;

				if(recv.equals("OK")){		//check if ok
					Serial.println(":: Server starded, running commands");
					started = true;
					break;
					
				} else if (recv.equals("SHUTDOWN")){
					Serial.println(":: Shutdown unprocessed");

        } else if (recv.equals("LEDON")){
          digitalWrite(13,HIGH);
          Serial.println("Turning led on pin 13 on");

        } else if (recv.equals("LEDOFF")){
          digitalWrite(13,LOW);
          Serial.println("Turning led on pin 13 off");
          
				} else {
					Serial.println(":: Command unknown");
					Serial.print(":: ");
					Serial.println(recv);
				}
			} else if(data_in){
				recv += c;
			} else {
				//do nothing, as we don't care unless it's between ~ and | 
			}
			
		}
		delay(1);
	}
	
}

void loop() {
	// put your main code here, to run repeatedly:

	Serial.print("Arduino One has been running for ");
	Serial.print(millis(),DEC);
	Serial.println(" milliseconds");



    while(Serial.available()){      //while / if serial data
      char c = Serial.read();     //read it

      if(c == '~'){         //start recv on ~
        data_in = true;
        recv = "";
        continue;         //read again

      } else if (data_in && c == '|'){  //finish recv on  |
        data_in = false;

        if (recv.equals("SHUTDOWN")){
          Serial.println(":: Shutdown unprocessed");

        } else if (recv.equals("LEDON")){
          digitalWrite(13,HIGH);
          Serial.println("Turning led on pin 13 on");

        } else if (recv.equals("LEDOFF")){
          digitalWrite(13,LOW);
          Serial.println("Turning led on pin 13 off");
          
        } else {
          Serial.println(":: Command unknown");
          Serial.print(":: ");
          Serial.println(recv);
        }
      } else if(data_in){
        recv += c;
      } else {
        //do nothing, as we don't care unless it's between ~ and | 
      }
      
    }


    

	delay(634);

}

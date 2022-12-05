using namespace std;

// function to generate and retrun random numbers.
int * getData(int left_sensor_value,int center_sensor_value, int right_sensor_value, int i) {

   static int  data[2];

   int value;
    switch (i)
    {
        case 0:  // This determines the steering

                if ((left_sensor_value == 1) && (center_sensor_value == 0) && (right_sensor_value) == 1) /* Car is centered on line */
                {
                        data[0] = 90;//return 90 /* Servo is set to middle position */
                        data[1] = 0; // steering
                        break;    //break
                }

                else if((left_sensor_value == 1) && (center_sensor_value == 1) && (right_sensor_value == 1))
                {
                        data[0] = 90;
                        data[1] = 0;
                        break;
                }

                else if((left_sensor_value == 0) && (center_sensor_value == 0) && (right_sensor_value == 0))
                {
                        data[0] = 90;
                        data[1] = 0;
                        break;
                }

                else if ((left_sensor_value == 0) && (center_sensor_value == 1) && (right_sensor_value == 1)) /* Car is left off the line */
                {
                        data[0] = 1; /* Servo is set to turn left */
                        data[1] = 0;
                        break; //break
                }

                else if ((left_sensor_value == 0) && (center_sensor_value == 0) && (right_sensor_value == 1)) /* Car is right off the line */
                {
                        data[0] = 1; /* Servo is set to turn right */
                        data[1] = 0;
                        break;
                }

                else if ((left_sensor_value == 1) && (center_sensor_value == 1) && (right_sensor_value == 0)) /* Car is right off the line */
                {
                        data[0] = 179; /* Servo is set to turn right */
                        data[1] = 0;
                        break;
                }
                             
                else if((left_sensor_value == 1) && (center_sensor_value == 0) && (right_sensor_value == 0))
                {
                        data[0] = 179;
                        data[1] = 0;
                        break;
                }


        case 1:  // This sets the speed

                if ((left_sensor_value == 1) && (center_sensor_value == 0) && (right_sensor_value) == 1) /* Car is centered on line */
                {
                        data[0] = 125;  //Speed  
                        data[1] = 2; 
                        break;    //break
                }
                else if((left_sensor_value == 1) && (center_sensor_value == 1) && (right_sensor_value == 1))
                {
                        data[0] = 0;
                        data[1] = 2;
                        break;
                }

                else if((left_sensor_value == 0) && (center_sensor_value == 0) && (right_sensor_value == 0))
                {
                        data[0] = 125;
                        data[1] = 2;
                        break;
                }

                else if ((left_sensor_value == 0) && (center_sensor_value == 1) && (right_sensor_value == 1)) /* Car is left off the line */
                {
                        data[0] = 118; /*Speed  */
                        data[1] = 2;
                        break; //break
                }

                else if ((left_sensor_value == 0) && (center_sensor_value == 0) && (right_sensor_value == 1)) /* Car is right off the line */
                {
                        data[0] = 118; /* Speed */
                        data[1] = 2;
                        break;
                }

                else if ((left_sensor_value == 1) && (center_sensor_value == 1) && (right_sensor_value == 0)) /* Car is right off the line */
                {
                        data[0] = 118; /* Speed */
                        data[1] = 2;
                        break;
                }
                
                else if((left_sensor_value == 1) && (center_sensor_value == 0) && (right_sensor_value == 0))  // Car is left off the line
                {
                        data[0] = 118;
                        data[1] = 2;
                        break;
                }

                
    }
    return data;   //return value
}


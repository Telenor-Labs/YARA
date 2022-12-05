int logic_value(int left_sensor_value, int right_sensor_value, int i)
{
    int value;
    switch (i)
    {
        case 0:
                if ((left_sensor_value == 1) && (right_sensor_value) == 1) /* Car is centered on line */
                {
                        value = 90;//return 90 /* Servo is set to middle position */
                        break;    //break
                }
                 else if ((left_sensor_value == 0) && (right_sensor_value == 1)) /* Car is left off the line */
                {
                        value = 125; /* Servo is set to turn left */
                        break; //break
                }
                else if ((left_sensor_value == 1) && (right_sensor_value == 0)) /* Car is right off the line */
                {
                        value = 45; /* Servo is set to turn right */
                        break;
                }
        case 1:
                // add code that changes acceleration based on steering logic
                                                                                                                                                                                                                                                                                                                                                                                                                                          
                value = 100; //Acceleration
                break;

    }
    return value;   //return value
}

int code(int i)
{
        int code;
        if(i==0)
        {
                code = 0;    //Ratt
        }
        else
        {
                code = 2;    //Pedal
        }
        return code; //Return code
}
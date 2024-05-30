#include <pins_arduino.h>
#include <Arduino.h>

// 取值 [1, 5]
int speed_rate = 3;


int ifStart = 0;
int timeo1 = 0;
int timeo2 = 0;
int timeo3 = 0;


int LEFT_MOTOR_ENA = 3;			//左电机使能 + PWM脉冲输出端口
int LEFT_MOTOR_IN1 = 2;			//左电机方向，0->前进，1->后退
int LEFT_MOTOR_IN2 = 4;			//左电机方向，1->前进，0->后退

int RIGHT_MOTOR_ENB = 6;		//右电机使能 + PWM脉冲输出端口
int RIGHT_MOTOR_IN3 = 7;		//右电机方向，0->前进，1->后退
int RIGHT_MOTOR_IN4 = 8;		//右电机方向，1->前进，0->后退

//循迹红外引脚定义
//TrackSensor1 TrackSensor2 TrackSensor3 TrackSensor4
//	   D10			D9			 D1			  D0
int TrackSensor1 = 10;  //使电调离自己最近：左侧第一个传感器为10
int TrackSensor2 = 9;  //使电调离自己最近：左侧第二个传感器为9
int TrackSensor3 = 1;  //使电调离自己最近：左侧第三个传感器为2
int TrackSensor4 = 0;  //使电调离自己最近：左侧第四个传感器为1

//定义各个循迹红外引脚采集的数据的变量
int TrackSensorValue1;
int TrackSensorValue2;
int TrackSensorValue3;
int TrackSensorValue4;


void setup()
{
	//初始化电机驱动IO口为输出方式
	pinMode(LEFT_MOTOR_ENA, OUTPUT);
	pinMode(LEFT_MOTOR_IN1, OUTPUT);
	pinMode(LEFT_MOTOR_IN2, OUTPUT);
	pinMode(RIGHT_MOTOR_ENB, OUTPUT);
	pinMode(RIGHT_MOTOR_IN3, OUTPUT);
	pinMode(RIGHT_MOTOR_IN4, OUTPUT);

	//定义四路循迹红外传感器为输入接口
	pinMode(TrackSensor1, INPUT);
	pinMode(TrackSensor2, INPUT);
	pinMode(TrackSensor3, INPUT);
	pinMode(TrackSensor4, INPUT);

	//四路循迹红外传感器初始化为高电平
	digitalWrite(TrackSensor1, HIGH);
	digitalWrite(TrackSensor2, HIGH);
	digitalWrite(TrackSensor3, HIGH);
	digitalWrite(TrackSensor4, HIGH);

}


void run(int left_speed, int right_speed)
{
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void brake(int ms)
{
	//左电机刹车
	digitalWrite(LEFT_MOTOR_ENA, LOW);
	//右电机刹车
	digitalWrite(RIGHT_MOTOR_ENB, LOW);

	delay(ms);
}


void left(int left_speed, int right_speed)
{
	//左电机刹车
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, LOW);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void right(int left_speed, int right_speed)
{
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机刹车
	digitalWrite(RIGHT_MOTOR_IN3, LOW);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void spin_left(int left_speed, int right_speed)
{
	//左电机后退
	digitalWrite(LEFT_MOTOR_IN1, HIGH);
	digitalWrite(LEFT_MOTOR_IN2, LOW);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void spin_right(int left_speed, int right_speed)
{
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机后退
	digitalWrite(RIGHT_MOTOR_IN3, LOW);
	digitalWrite(RIGHT_MOTOR_IN4, HIGH);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void route1(){
	timeo1 = 0;
	while(1){
		++timeo1;
		if(timeo1 == 250){
			++timeo2;
			timeo1 = 0;
		}
		if(timeo2 == 250){
			++timeo3;
			timeo2 = 0;
		}
		// 起始时
		if(ifStart == 0 && timeo2 < 70){
			run(51*speed_rate, 51*speed_rate);
			continue;
		}
		ifStart = 1;

		TrackSensorValue1 = digitalRead(TrackSensor1);
		TrackSensorValue2 = digitalRead(TrackSensor2);
		TrackSensorValue3 = digitalRead(TrackSensor3);
		TrackSensorValue4 = digitalRead(TrackSensor4);

		if (TrackSensorValue1 == HIGH && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW && TrackSensorValue4 == LOW)
		{
			spin_left(40*speed_rate, 30*speed_rate);
		}
		else if (TrackSensorValue1 == LOW && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW && TrackSensorValue4 == HIGH)
		{
			spin_right(30*speed_rate, 40*speed_rate);
		}
//		else if (TrackSensorValue2 == HIGH && TrackSensorValue3 == LOW)
//		{
//			right(0, 44*speed_rate);
//		}
//		else if (TrackSensorValue2 == LOW && TrackSensorValue3 == HIGH)
//		{
//			left(44*speed_rate, 0);
//		}
		else if(TrackSensorValue1 == HIGH && TrackSensorValue2 == HIGH && TrackSensorValue3 == HIGH && TrackSensorValue4 == HIGH){
			while (1){
				brake(1);
			}
		}
		else if (TrackSensorValue2 == HIGH || TrackSensorValue3 == HIGH)
		{
			run(51*speed_rate, 51*speed_rate);
		}
		delay(2);
	}
}

void route2(){
	timeo1 = 0;
	while(1){
		++timeo1;
		if(timeo1 == 250){
			++timeo2;
			timeo1 = 0;
		}
		if(timeo2 == 250){
			timeo2 = 0;
		}
		// 起始时
		if(ifStart == 0 && timeo2 < 30){
			run(51*speed_rate, 51*speed_rate);
			continue;
		}
		ifStart = 1;

		TrackSensorValue1 = digitalRead(TrackSensor1);
		TrackSensorValue2 = digitalRead(TrackSensor2);
		TrackSensorValue3 = digitalRead(TrackSensor3);
		TrackSensorValue4 = digitalRead(TrackSensor4);


		if (TrackSensorValue1 == HIGH && (TrackSensorValue3 == HIGH ||  TrackSensorValue4 == HIGH))
		{
			spin_left(51*speed_rate, 51*speed_rate);
			delay(150);
		}
		else if (TrackSensorValue1 == HIGH)
		{
			spin_left(51*speed_rate, 51*speed_rate);
			delay(2);
		}
		else if ((TrackSensorValue1 == HIGH || TrackSensorValue2 == HIGH) &&  TrackSensorValue4 == HIGH)
		{
			spin_right(51*speed_rate, 51*speed_rate);
			delay(150);
		}
		else if (TrackSensorValue4 == HIGH  && ifStart == 2)
		{
			spin_right(51*speed_rate, 51*speed_rate);
			delay(2);
		}
		else if (TrackSensorValue1 == LOW && TrackSensorValue2 == HIGH && TrackSensorValue3 == LOW && TrackSensorValue4 == LOW)
		{
			left(44*speed_rate, 44*speed_rate);
		}
		else if (TrackSensorValue1 == LOW && TrackSensorValue2 == LOW && TrackSensorValue3 == HIGH && TrackSensorValue4 == LOW)
		{
			right(44*speed_rate, 44*speed_rate);
		}
		else if (TrackSensorValue2 == HIGH && TrackSensorValue3 == HIGH)
		{
			run(30*speed_rate, 30*speed_rate);
		}
	}
}


void loop()
{
	while(1){
//		route2();
		++timeo1;
		if(timeo1 == 250){
			++timeo2;
			timeo1 = 0;
		}
		if(timeo2 == 250){
			++timeo3;
			timeo2 = 0;
		}
		// 起始时
		if(ifStart == 0 && timeo2 < 70){
			run(51*speed_rate, 51*speed_rate);
			continue;
		}
		ifStart = 1;

		TrackSensorValue1 = digitalRead(TrackSensor1);
		TrackSensorValue2 = digitalRead(TrackSensor2);
		TrackSensorValue3 = digitalRead(TrackSensor3);
		TrackSensorValue4 = digitalRead(TrackSensor4);

		if (TrackSensorValue1 == HIGH && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW && TrackSensorValue4 == LOW)
		{
			spin_left(40*speed_rate, 30*speed_rate);
		}
		else if (TrackSensorValue1 == LOW && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW && TrackSensorValue4 == HIGH)
		{
			spin_right(30*speed_rate, 40*speed_rate);
		}
//		else if (TrackSensorValue2 == HIGH && TrackSensorValue3 == LOW)
//		{
//			right(0, 44*speed_rate);
//		}
//		else if (TrackSensorValue2 == LOW && TrackSensorValue3 == HIGH)
//		{
//			left(44*speed_rate, 0);
//		}
		else if(TrackSensorValue1 == HIGH && TrackSensorValue2 == HIGH && TrackSensorValue3 == HIGH && TrackSensorValue4 == HIGH){
			while (1){
				brake(1);
			}
		}
		else if (TrackSensorValue2 == HIGH || TrackSensorValue3 == HIGH)
		{
			run(51*speed_rate, 51*speed_rate);
		}
		delay(2);
	}
}
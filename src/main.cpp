#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>


void timerIsr();

// 取值 [1, 5]
int speed_rate = 3;

int LEFT_MOTOR_ENA = 3;            //左电机使能 + PWM脉冲输出端口
int LEFT_MOTOR_IN1 = 2;            //左电机方向，0->前进，1->后退
int LEFT_MOTOR_IN2 = 4;            //左电机方向，1->前进，0->后退

int RIGHT_MOTOR_ENB = 6;        //右电机使能 + PWM脉冲输出端口
int RIGHT_MOTOR_IN3 = 7;        //右电机方向，0->前进，1->后退
int RIGHT_MOTOR_IN4 = 8;        //右电机方向，1->前进，0->后退

//循迹红外引脚定义
//TrackSensor1 TrackSensor2 TrackSensor3 TrackSensor4
//	   D12			D11			 D10		  D9
int TrackSensor1 = 12;  //使电调离自己最近：左侧第一个传感器为10
int TrackSensor2 = 11;  //使电调离自己最近：左侧第二个传感器为9
int TrackSensor3 = 10;  //使电调离自己最近：左侧第三个传感器为2
int TrackSensor4 = 9;  //使电调离自己最近：左侧第四个传感器为1

//定义各个循迹红外引脚采集的数据的变量
int TrackSensorValue1;
int TrackSensorValue2;
int TrackSensorValue3;
int TrackSensorValue4;

// 计时，单位为毫秒
volatile int running = 0;
volatile int timerCnt = 0;
volatile int interval = 0;
volatile int lineCnt = 0;
SoftwareSerial bluetoothSerial(0, 1); // RX, TX


void setup() {
	// 设置定时器1
	Timer1.initialize(100000); // 设置定时器间隔为0.1秒（100000微秒）
	Timer1.attachInterrupt(timerIsr); // 将timerIsr函数附加为中断服务例程

	// 设置蓝牙
	bluetoothSerial.begin(9600); // 设置蓝牙串口的波特率

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


void timerIsr() {
	timerCnt += 1;
	if(timerCnt % 10 == 0){
		bluetoothSerial.print(String(timerCnt / 10));
	}
	if (interval > 0) {
		interval -= 1;
	}
	if (timerCnt == 1000) {
		timerCnt = 0;
	}
}


void run(int left_speed, int right_speed) {
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void brake(int ms) {
	//左电机刹车
	digitalWrite(LEFT_MOTOR_ENA, LOW);
	//右电机刹车
	digitalWrite(RIGHT_MOTOR_ENB, LOW);

	delay(ms);
}


void left(int left_speed, int right_speed) {
	//左电机刹车
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, LOW);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void right(int left_speed, int right_speed) {
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机刹车
	digitalWrite(RIGHT_MOTOR_IN3, LOW);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void spin_left(int left_speed, int right_speed) {
	//左电机后退
	digitalWrite(LEFT_MOTOR_IN1, HIGH);
	digitalWrite(LEFT_MOTOR_IN2, LOW);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机前进
	digitalWrite(RIGHT_MOTOR_IN3, HIGH);
	digitalWrite(RIGHT_MOTOR_IN4, LOW);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void spin_right(int left_speed, int right_speed) {
	//左电机前进
	digitalWrite(LEFT_MOTOR_IN1, LOW);
	digitalWrite(LEFT_MOTOR_IN2, HIGH);
	analogWrite(LEFT_MOTOR_ENA, left_speed);
	//右电机后退
	digitalWrite(RIGHT_MOTOR_IN3, LOW);
	digitalWrite(RIGHT_MOTOR_IN4, HIGH);
	analogWrite(RIGHT_MOTOR_ENB, right_speed);
}


void route1() {
	// 起始时
	if (timerCnt < 7) {
		run(51 * speed_rate, 51 * speed_rate);
		return;
	}

	TrackSensorValue1 = digitalRead(TrackSensor1);
	TrackSensorValue2 = digitalRead(TrackSensor2);
	TrackSensorValue3 = digitalRead(TrackSensor3);
	TrackSensorValue4 = digitalRead(TrackSensor4);

	if (TrackSensorValue1 == HIGH && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW && TrackSensorValue4 == LOW) {
		spin_left(40 * speed_rate, 30 * speed_rate);
	} else if (TrackSensorValue1 == LOW && TrackSensorValue2 == LOW && TrackSensorValue3 == LOW &&
			   TrackSensorValue4 == HIGH) {
		spin_right(30 * speed_rate, 40 * speed_rate);
	}
//		else if (TrackSensorValue2 == HIGH && TrackSensorValue3 == LOW)
//		{
//			right(0, 44*speed_rate);
//		}
//		else if (TrackSensorValue2 == LOW && TrackSensorValue3 == HIGH)
//		{
//			left(44*speed_rate, 0);
//		}
	else if (TrackSensorValue1 == HIGH && TrackSensorValue2 == HIGH && TrackSensorValue3 == HIGH &&
			 TrackSensorValue4 == HIGH && timerCnt > 130) {
		brake(1);
		running = 0;
		return;
	} else if (TrackSensorValue2 == HIGH || TrackSensorValue3 == HIGH) {
		run(51 * speed_rate, 51 * speed_rate);
	}
	delay(2);
}


void route2() {
	// 起始时 0.3s
	if (timerCnt < 3) {
		run(51 * speed_rate, 51 * speed_rate);
		return;
	}
	if ((timerCnt > 90 && timerCnt < 110) || timerCnt > 170) {
		if (TrackSensorValue1 + TrackSensorValue2 + TrackSensorValue3 + TrackSensorValue4 >= 3 && interval == 0) {
			bluetoothSerial.print('o');
			interval = 5;
			lineCnt++;
			if (lineCnt == 1) {
				bluetoothSerial.print('a');
				run(20 * speed_rate, 20 * speed_rate);
				delay(200);
			} else if (lineCnt == 2) {
				bluetoothSerial.print('b');
				goto right;
			} else if (lineCnt == 3) {
				bluetoothSerial.print('c');
				brake(1);
				running = 0;
				return;
			}
		}
	}

	TrackSensorValue1 = digitalRead(TrackSensor1);
	TrackSensorValue2 = digitalRead(TrackSensor2);
	TrackSensorValue3 = digitalRead(TrackSensor3);
	TrackSensorValue4 = digitalRead(TrackSensor4);

	if (TrackSensorValue1 == HIGH && (TrackSensorValue3 == HIGH || TrackSensorValue4 == HIGH)) {
		if(timerCnt < 110 && timerCnt > 90 && lineCnt == 0){
			goto direct;
		}
		spin_left(51 * speed_rate, 51 * speed_rate);
		delay(150);
	} else if (TrackSensorValue1 == HIGH) {
		if(timerCnt < 110 && timerCnt > 90 && lineCnt == 0){
			goto direct;
		}
		spin_left(51 * speed_rate, 51 * speed_rate);
		delay(2);
	} else if ((/*TrackSensorValue1 == HIGH ||*/ TrackSensorValue2 == HIGH) && TrackSensorValue4 == HIGH &&
			   timerCnt > 7) {
		right:
		if(timerCnt < 110 && timerCnt > 90 && lineCnt == 0){
			goto direct;
		}
		spin_right(51 * speed_rate, 51 * speed_rate);
		delay(150);
	} else if (TrackSensorValue4 == HIGH && timerCnt > 7) {
		if(timerCnt < 110 && timerCnt > 90 && lineCnt == 0){
			goto direct;
		}
		spin_right(51 * speed_rate, 51 * speed_rate);
		delay(2);
	} else if (TrackSensorValue1 == LOW && TrackSensorValue2 == HIGH && TrackSensorValue3 == LOW &&
			   TrackSensorValue4 == LOW) {
		if (timerCnt < 110 && timerCnt > 70 && lineCnt == 0) {
			left(30 * speed_rate, 30 * speed_rate);
			return;
		}
		left(44 * speed_rate, 44 * speed_rate);
	} else if (TrackSensorValue1 == LOW && TrackSensorValue2 == LOW && TrackSensorValue3 == HIGH &&
			   TrackSensorValue4 == LOW) {
		if (timerCnt < 110 && timerCnt > 70 && lineCnt == 0) {
			right(30 * speed_rate, 30 * speed_rate);
			return;
		}
		right(44 * speed_rate, 44 * speed_rate);
	} else if (TrackSensorValue2 == HIGH && TrackSensorValue3 == HIGH) {
		direct:
		run(30 * speed_rate, 30 * speed_rate);
	}
}


void ifStart() {
	while (1) {
		if (bluetoothSerial.available()) {
			bluetoothSerial.read();
			timerCnt = 0;
			lineCnt = 0;
			break;
		}
	}
}

void loop() {
	if (running == 0) {
		ifStart();
		running = 1;
	}
//	route1();
	route2();
}
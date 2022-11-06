# 1 "d:\\Arduino\\project\\Vehicle\\main\\src\\src.ino"
# 2 "d:\\Arduino\\project\\Vehicle\\main\\src\\src.ino" 2
# 3 "d:\\Arduino\\project\\Vehicle\\main\\src\\src.ino" 2
# 4 "d:\\Arduino\\project\\Vehicle\\main\\src\\src.ino" 2
void abort(const char * words)
{
    Serial.println("[Error]:");
    Serial.println(words);
    Serial.println("\n");
}
void log(const char * words)
{
    Serial.println("[Log]:");
    Serial.println(words);
    Serial.println("\n");
}
class PIDD
{
    double err[3],offset,kp,kt,kd;
    public:
    PIDD():offset(0),kp(1),kt(1),kd(-1)
    {
        err[0]=err[1]=err[2]=0;
    }
    void update(double error)
    {
        err[0]=err[1];
        err[1]=err[2];
        err[2]=error;
    }
    int value()
    {
        offset+=kp*(err[2]-err[1])+kt*err[2]+kd*(err[2]-2*err[1]+err[0]);
        return int(offset);
    }
    void clean()
    {
        err[0]=err[1]=err[2]=0;
    }
};
class vehicle
{
    public:
    static const byte IIC_ADDRESS = 0x3F;
    enum wheel_position{
        LF=3,RR=4,RF=2,LR=1
    };
    //M2 RF M3 LF M4 LR M1 RR
    enum sensors{
        INPUT_PIN_S0,INPUT_PIN_S1,INPUT_PIN_S2,INPUT_PIN_S3,INPUT_PIN_S4
    };
    enum direction{
        Forward,Backward,Lean,Leftward,Clockwise,Anticlock
    };
    enum Servors{
        One=0,Two=7,Three=3,Four=4
    };
    enum offset{
        Toleft,Left,Right,Toright,Mean,Line,outline,Error
    };
    offset lastchoice;
    const char *name;
    QGPMaker_MotorShield *AFMS;
    QGPMaker_DCMotor *wheels[5];
    SensorBar *io;
    int delta;
    QGPMaker_Servo *servos[8];//uninit'd
    QGPMaker_Encoder *encoders[5];
    PIDD pids[5];
    int speeds[5];
    void init(){
        log("Vehicle Created");
        log(name);
        speeds[LF]=speeds[LR]=speeds[RF]=speeds[RR]=0;
        AFMS=new QGPMaker_MotorShield ();
        AFMS->begin(50);
        wheels[LF]=AFMS->getMotor(LF);
        wheels[LR]=AFMS->getMotor(LR);
        wheels[RF]=AFMS->getMotor(RF);
        wheels[RR]=AFMS->getMotor(RR);
        io=new SensorBar();
        if(!io->begin(IIC_ADDRESS))
        {
            abort("I2C Failed");
            //while(1);
        }
        else
            log("I2C Started");
        io->pinMode(INPUT_PIN_S0, 0x0);
        io->pinMode(INPUT_PIN_S1, 0x0);
        io->pinMode(INPUT_PIN_S2, 0x0);
        io->pinMode(INPUT_PIN_S3, 0x0);
        io->pinMode(INPUT_PIN_S4, 0x0);
        servos[One]=AFMS->getServo(One);
        servos[Two]=AFMS->getServo(Two);
        servos[Three]=AFMS->getServo(Three);
        servos[Four]=AFMS->getServo(Four);
        encoders[LF]=new QGPMaker_Encoder(LF);
        encoders[LR]=new QGPMaker_Encoder(LR);
        encoders[RF]=new QGPMaker_Encoder(RF);
        encoders[RR]=new QGPMaker_Encoder(RR);

    }
    vehicle(const char *carname):name(carname),io(nullptr){
        lastchoice=Mean;
    }

    void forward(wheel_position id)
    {
        switch (id)
        {
            case LF:
                wheels[id]->run(1);
                break;
            case LR:
                wheels[id]->run(1);
                break;
            case RF:
                wheels[id]->run(2);
                break;
            case RR:
                wheels[id]->run(2);
                break;
            default:
                abort("[Forward] Format Wrong");
                break;
        }
        return;
    }
    void backward(wheel_position id)
    {
        switch (id)
        {
            case LF:
                wheels[id]->run(2);
                break;
            case LR:
                wheels[id]->run(2);
                break;
            case RF:
                wheels[id]->run(1);
                break;
            case RR:
                wheels[id]->run(1);
                break;
            default:
                abort("[Backward] Format Wrong");
                break;
        }
        return;
    }
    void stop(wheel_position id)
    {
        wheels[id]->run(3);
    }
    void Brake()
    {
        stop(LF);
        stop(LR);
        stop(RR);
        stop(RF);
    }
    void release(wheel_position id)
    {
        wheels[id]->run(4);
    }
    void restart_back(wheel_position id)
    {
        stop(id);
        delay(2);
        backward(id);
    }
    void run(direction dir)
    {
        switch (dir)
        {
            case Forward:
                forward(LR);
                forward(LF);
                forward(RF);
                forward(RR);
                break;
            case Backward:
                backward(LR);
                backward(LF);
                backward(RF);
                backward(RR);
                break;
            case Lean:
                stop(LF);
                stop(RR);
                forward(LR);
                forward(RF);
                break;
            case Leftward:
                backward(LF);
                backward(RR);
                forward(LR);
                forward(RF);
                break;
            case Clockwise:
                forward(LF);
                forward(LR);
                backward(RR);
                backward(RF);
            case Anticlock:
                forward(RF);
                forward(RR);
                backward(LR);
                backward(LF);
            default:
                break;
        }
    }
    void Speed(wheel_position id,uint8_t speed)
    {
        wheels[id]->setSpeed(speed);
        speeds[id]=speed;
    }
    void setSpeed(uint8_t speed)
    {
        Speed(LR,speed);
        Speed(LF,speed);
        Speed(RF,speed);
        Speed(RR,speed);
    }
    int detect()
    {
        int detect_ans=0;
        for(int i=0;i<5;++i)
            detect_ans=2*detect_ans+(io->digitalRead(i)^1);
        return detect_ans;
    }
    offset judge_position()
    {
        int p=detect();
        bool on[5];
        on[0]=p&16;
        on[1]=p&8;
        on[2]=p&4;
        on[3]=p&2;
        on[4]=p&1;
        log("[Detecting]");
        for(int i=0;i<5;++i)
            if(on[i])
                Serial.print(i);
        Serial.println(' ');
        if(on[0]&&on[5])
            return Line;
        if(on[0])
            return Toright;
        if(on[1])
            return Right;
        if(on[4])
            return Toleft;
        if(on[3])
            return Left;
        if(!on[2])
            return outline;
        return Mean;
    }
    void angle(Servors id,uint8_t Angle)
    {
        servos[id]->writeServo(Angle);
    }
    volatile int RPM(wheel_position id)
    {
        volatile int rpm=encoders[id]->getRPM();
        return rpm;
    }
    void adjust(wheel_position id,int target_r,int tolerance=1)
    {
        pids[id].clean();
        volatile int rpm=0;
        uint8_t speed=0;
        int delta=0;
        volatile int loss=rpm-target_r;
        //Serial.println(rpm);
        //Serial.println(loss);
        while(((loss)>0?(loss):-(loss))>tolerance)
        {
            rpm=encoders[id]->getRPM();
            loss=rpm-target_r;
            pids[id].update(loss);
            delta=pids[id].value();
            speed+=delta;
            Speed(id,speed);
            delay(50);
            Serial.println(loss);
            if(speed<0)
                abort("[PID Error]speed<0");
        }
    }
    void setrpm(int target_r,int tolerance=1)
    {
        adjust(LF,target_r,tolerance);
        adjust(LR,target_r,tolerance);
        adjust(RR,target_r,tolerance);
        adjust(RF,target_r,tolerance);
    }
    void restart(wheel_position id)
    {
        stop(id);
        delay(1);
        forward(id);
    }
    void adjust_line_Speed()
    {
        offset dir=judge_position();
        if(dir==offset::Right||dir==offset::Left)
            delta=70;
        else
            delta=170;
        if(dir==offset::Toright)
            log("Too Left");
        if(dir==offset::Toleft)
            log("Too Right");

        if(dir==offset::outline)
            dir=offset::Mean;
        if(dir==offset::Mean)
        {
            run(Forward);
            Speed(LR,30);
            Speed(LF,30);
            Speed(RR,43);
            Speed(RF,43);
        }
        if (dir==offset::Toright||dir==offset::Right)
        {
            //Speed(LR,255);
            restart(LF);
            restart(LR);
            //Speed(LF,100+delta);
            //Speed(RR,10);
            //Speed(RF,10);
            //restart_back(RF);
            //restart_back(RR);
            stop(RF);
            stop(RR);
            Speed(LR,30+delta);
            Speed(LF,30+delta);
            //Speed(RF,100+delta);
            //Speed(RR,75+delta);
            //Speed()
            //backward(RR);
            //backward(RF);
            //forward(LF);
            //Speed(RF,150);
            //Speed(RR,50+delta);
            //Speed(RR,100+delta);
            //Speed(LF,200);
        }
        if (dir==offset::Toleft||dir==offset::Left)
        {
            restart(RF);
            restart(RR);
            //restart_back(LF);
            //restart_back(LR);
            //Speed(LF,100+delta);
            //Speed(RR,10);
            //Speed(RF,10);
            stop(LF);
            stop(LR);
            Speed(RR,43+delta);
            Speed(RF,43+delta);
            //Speed(LF,75+delta);
            //Speed(LR,50+delta);
        }
        if (dir==offset::Line)
        {
            log("Line");
            Brake();
            delay(20000);
            run(Forward);
        }
        lastchoice=dir;
    }
};
vehicle car("hello world");
void run(){
    car.run(vehicle::direction::Forward);
    car.angle(vehicle::Servors::One,30);
    car.angle(vehicle::Servors::Two,50);
    car.angle(vehicle::Servors::Three,130);
    car.angle(vehicle::Servors::Four,0);
    car.Speed(vehicle::wheel_position::LF,48);
    car.Speed(vehicle::wheel_position::LR,48);
    car.Speed(vehicle::wheel_position::RF,45);
    car.Speed(vehicle::wheel_position::RR,45);
}
void setup() {

    log("begin");
  // put your setup code here, to run once:
    Serial.begin(9600);
    car.init();
    //car.setrpm(50)

}
void adjust(int angel1,int angle2,int d1,int d2)
{
    int n=100;
    for(int i=0;i<n;i++)
    {
        delay(50);
        car.angle(vehicle::Servors::Two,angel1+d1*i/n);
        car.angle(vehicle::Servors::Three,angle2+d2*i/n);
    }
}
void Grab()
{
    car.Brake();
    car.angle(vehicle::Servors::One,40);
    car.angle(vehicle::Servors::Two,20);
    car.angle(vehicle::Servors::Three,60);
    delay(5000);
    car.angle(vehicle::Servors::Two,50);
    car.angle(vehicle::Servors::Three,140);
    delay(1000);
    car.angle(vehicle::Servors::One,80);
    car.angle(vehicle::Servors::Four,0);
    delay(2000);
    car.angle(vehicle::Servors::One,90);
    car.angle(vehicle::Servors::Two,10);
    car.angle(vehicle::Servors::Three,93);
    delay(2000);
    car.angle(vehicle::Servors::Four,40);
    delay(2000);
    car.angle(vehicle::Servors::One,40);
    car.angle(vehicle::Servors::Two,20);
    car.angle(vehicle::Servors::Three,60);
    delay(5000);
    car.angle(vehicle::Servors::One,85);
    car.angle(vehicle::Servors::Two,10);
    car.angle(vehicle::Servors::Three,93);
    delay(2000);
    car.angle(vehicle::Servors::Four,0);
    delay(1000);
}
int cnt=0;
void loop() {
    cnt+=1;

    //Grab();
    //adjust(50,50,-40,40);
    //Serial.print(cnt&1);
    //car.forward();
    car.run(vehicle::direction::Forward);
    car.adjust_line_Speed();
    /*delay(1000);

    car.angle(vehicle::Servors::Three,0);

    */
# 453 "d:\\Arduino\\project\\Vehicle\\main\\src\\src.ino"
   //delay(1000);
   //car.angle(vehicle::Servors::Three,120);
   //delay(5000);
}

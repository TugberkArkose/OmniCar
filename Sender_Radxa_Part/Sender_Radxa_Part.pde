import processing.video.*;
import processing.serial.*;    
import javax.imageio.*;
import java.awt.image.*; 
import java.io.*;
import java.net.*;
Serial myPort;   
boolean UpPressed = false;
boolean DownPressed = false;
boolean RightPressed = false;
boolean LeftPressed = false;
long time;
long wait = 1000;
byte[]buf = new byte[64];
int clientPort = 9100; 
int clientPort_compass = 9110;
DatagramSocket ds; 
DatagramSocket ds2; 
Capture cam;

void setup() {
  size(320,240);
  time = millis();
  size(200, 200);                                  
  String portName = Serial.list()[0];              
  myPort = new Serial(this, Serial.list()[1], 9600);
  try {
    ds = new DatagramSocket();
    ds2=new DatagramSocket();
  } catch (SocketException e) {
    e.printStackTrace();
  }
  cam = new Capture( this, 160,120,30);
  cam.start();
}
void captureEvent( Capture c ) {
  c.read();
  broadcast(c);
}
void draw() {
  background(255);        
  frame.setAlwaysOnTop(true); 
  if ( UpPressed && !RightPressed && !LeftPressed) 
  {  
    myPort.write("w");
    textSize(32);
    text("W", 10, 30); 
    fill(0, 102, 153);
  } else if (RightPressed && UpPressed)
  {
    myPort.write("e");
    textSize(32);
    text("E", 10, 30); 
    fill(0, 102, 153);
  } else if (LeftPressed && UpPressed)
  {
    myPort.write("q");
    textSize(32);
    text("Q", 10, 30); 
    fill(0, 102, 153);
  } else if ( DownPressed  && !RightPressed && !LeftPressed) 
  {
    myPort.write("s");
    textSize(32);
    text("S", 10, 30); 
    fill(0, 102, 153);
  } else if (RightPressed && DownPressed)
  {
    myPort.write("x");
    textSize(32);
    text("X", 10, 30); 
    fill(0, 102, 153);
  } else if (LeftPressed && DownPressed)
  {
    myPort.write("z");
    textSize(32);
    text("Z", 10, 30); 
    fill(0, 102, 153);
  } else if (RightPressed)
  {
    myPort.write("d");
    textSize(32);
    text("D", 10, 30); 
    fill(0, 102, 153);
  } else if (LeftPressed)
  {
    myPort.write("a");
    textSize(32);
    text("A", 10, 30); 
    fill(0, 102, 153);
  }
  else
    myPort.write("n");
    
 long current_time = millis();
 
 if(current_time - time >= wait)
 {
   if(myPort.available() > 0)
   {
    String compass = myPort.readString();
    buf = compass.getBytes();
    try
    {
    ds2.send(new DatagramPacket(buf,buf.length, InetAddress.getByName("10.44.0.16"),9110)); // IP buraya gelecek !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    catch (Exception e) 
    {
    e.printStackTrace();
    }
    myPort.clear();
    time = current_time;
   }
 }
    //delay(500);
}
void broadcast(PImage img) {
  BufferedImage bimg = new BufferedImage( img.width,img.height, BufferedImage.TYPE_INT_RGB );
  img.loadPixels();
  bimg.setRGB( 0, 0, img.width, img.height, img.pixels, 0, img.width);
  ByteArrayOutputStream baStream  = new ByteArrayOutputStream();
  BufferedOutputStream bos    = new BufferedOutputStream(baStream);
  try {
    ImageIO.write(bimg, "jpg", bos);
  } 
  catch (IOException e) {
    e.printStackTrace();
  }
  byte[] packet = baStream.toByteArray();
  try {
    ds.send(new DatagramPacket(packet,packet.length, InetAddress.getByName("10.44.0.16"),clientPort)); // ip buraya gelecek !!!!!!!!!
  } 
  catch (Exception e) {
    e.printStackTrace();
  }
}
void keyPressed()
{
  if (keyCode==UP && !UpPressed)
    UpPressed=true;
  if (keyCode==DOWN && !DownPressed)
    DownPressed=true;
  if (keyCode==LEFT && !LeftPressed)
    LeftPressed = true;
  if (keyCode == RIGHT && !RightPressed)
    RightPressed = true;
}
void keyReleased()
{
  if (keyCode==UP)
    UpPressed=false;
  if (keyCode==DOWN )
    DownPressed=false;
  if (keyCode==LEFT )
    LeftPressed = false;
  if (keyCode == RIGHT )
    RightPressed = false;
}

import gab.opencv.*;
import processing.video.*;
import java.awt.*;
import org.opencv.core.MatOfPoint;
import processing.serial.*;

Capture cam;
Capture video;

OpenCV opencv;
int[] outputValues;
Serial ledSerial;
int count;
String inString;

void setup() {
  String[] cameras = Capture.list();
  size(640, 480);
  video = new Capture(this, 640/2, 480/2);

  //not being used. TODO: remove if not needed
  for (int x = 0; x < cameras.length; x++){}

  cam = new Capture(this);
  opencv = new OpenCV(this, 640/2, 480/2);//640 480
  count = 0;

  //ledSerial = new Serial(this, "COM4");

  outputValues = new int[90];
  cam.start();
  video.start();

  opencv.startBackgroundSubtraction(5, 3, 0.5);
}

void draw() {
  scale(2);
  opencv.loadImage(video);
  PImage img = createImage(640, 480, RGB);
  PImage edgeImage = createImage(5, 480, RGB);
  img.loadPixels();

  for (int i = 0; i < img.pixels.length; i++) {
     img.pixels[i] = color(255, 255, 255, 100);
  }
  img.updatePixels();
  image(img, 0, 0 );
  edgeImage.loadPixels();

  for (int i = 0; i < edgeImage.pixels.length; i++) {
     edgeImage.pixels[i] = color(255, 255, 255, 255);
  }
  img.updatePixels();
  image(img, 0, 0 );
  image(edgeImage, 0, 0);
  opencv.updateBackground();
  noFill();
  stroke(255, 0, 0);
  strokeWeight(2);
  ArrayList <Contour> contours;
  contours = opencv.findContours();

  for (Contour contour : opencv.findContours()) {
    contour.draw();
  }

  int valueCount = 0;

  for (int i = 0; i < 480; i += 6){
    int redCount = 0;
    for (int j = 0; j < 640; j++){
      if (blue((get (j, i))) < 10 ){
        redCount ++;
      }
    }
    if (redCount > 30){
      outputValues[valueCount] = (i/6);
      valueCount++;
      set(1, i, color(0, 0, 255));
      set(2, i, color(0, 0, 255));
      set(3, i, color(0, 0, 255));
    }
  }

  byte[] ledData =  new byte[90];
  boolean empty;
  empty = true;
  for (int y = 0; y < 90; y++){
    ledData[(y)] = byte(90 - outputValues[(y)]);
    if (ledData[y] > 0){
      empty = false;
    }
    outputValues[y] = 0;
  }

  ledData[0] = byte(100);
  if (empty){
    ledData[0] = byte(101);
  }
  //else { }
  if (!empty){
    ledData[89] = byte(102);
  }
  ledSerial.write(ledData); //-----------------------------------------------
}

void captureEvent(Capture c) {
  c.read();
}

void serialEvent(Serial p) {
  inString = p.readString();
  println(inString);
} 

import processing.serial.*;
//import java.io.*; java har ändrat 
import java.io.FileReader;
import java.io.FileNotFoundException;
int mySwitch=0;
int counter=0;
String [] subtext;
Serial myPort; 
int byteRead;

PrintWriter output;
int amountOfNumbers = 0;

void setup() {
  subtext = new String [0];
  output = createWriter("D:\\Saker jag inte får bli av med\\Digital teknik och 8-bit computers\\New 8 bit computer - Green Stone\\IO 8-bit computer\\data_from_8-bit_computer.txt"); 
  
  //Create a switch that will control the frequency of text file reads.
  //When mySwitch=1, the program is setup to read the text file.
  //This is turned off when mySwitch = 0
  mySwitch=1;

  //Open the serial port for communication with the Arduino
  //Make sure the COM port is correct (look in your Arduino sketch, tools->port) 
  //and same baudrate as the Arduino sketch (57600)
  myPort = new Serial(this, "COM11", 57600);
  myPort.bufferUntil('\n');
  delay(1500);
}

void draw() {
 
   if(myPort.available() > 0)
   {
     //writeData();   //to textfile                                           // toggle between readData() and writeData()
   }
   
  if (mySwitch>0) { 
    /*The readData function can be found later in the code.
     This is the call to read a CSV file on the computer hard-drive. */
    readData("D:\\Saker jag inte får bli av med\\Digital teknik och 8-bit computers\\New 8 bit computer - Green Stone\\IO 8-bit computer\\Programs for 8-bit comuter.txt"); 
                                                                                      //C:/Users/It/Desktop/Code.txt //lapptop   // toggle between readData() and writeData()
    
    /*The following switch prevents continuous reading of the text file, until
     we are ready to read the file again. */
    mySwitch=0;
  }
  /*Only send new data. This IF statement will allow new data to be sent to
   the arduino. */
  if (counter<subtext.length) {

    myPort.write(subtext[counter]); //send a byte to the Arduino via serial port

    delay(20);


    if (myPort.available() > 0)
    { 
      //print(char(myPort.read()) + ", ");
      //print(char(myPort.read()) + ", ");
    
      print(Integer.toHexString(myPort.read()) + ": ");
        
      //delay(1);
    }

    //Increment the counter so that the next number is sent to the arduino.
    counter++;
  } 
} 


/* The following function will read from a CSV or TXT file */
void readData(String myFileName) {

  File file=new File(myFileName);
  BufferedReader br=null;

  try {
    br=new BufferedReader(new FileReader(file));
    String text=null;

    /* keep reading each line until you get to the end of the file */
    while ((text=br.readLine())!=null) {
      /* Spilt each line up into bits and pieces using a comma as a separator */
      subtext = splitTokens(text, ",");
    }
  }
  catch(FileNotFoundException e) {
    e.printStackTrace();
  }
  catch(IOException e) {
    e.printStackTrace();
  }
  finally {
    try {
      if (br != null) {
        br.close();
      }
    } 
    catch (IOException e) {
      e.printStackTrace();
    }
  }
}

/* The following function will write from 8-bit computer to a TXT file */
void writeData() {
  
  
  int number = myPort.read();

  if(amountOfNumbers >= 255)
  {
    output.flush(); // Writes the remaining data to the file
    output.close(); // Finishes the file
  }
  else {
    output.println(number); // Write the coordinate to the file
    print(number+", ");
    amountOfNumbers++;
  }
  //delay(1);
}

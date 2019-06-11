#define RS    8
#define EN    7
#define D4    6
#define D5    5
#define D6    4
#define D7    3

#define B 0 //binary
#define H 1 //hexadecimal



byte pins[] = {D4, D5, D6, D7};
byte customChar[8] = {
  0b00000,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000,
  0b00000
};
byte customChar2[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b10001,
  0b01010,
  0b00100
};

void setup() {
  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  delay(5);
  Serial.begin(9600);
  lcd_init();
  draw_char(customChar, 0);
  draw_char(customChar2, 1);
}

void loop() {
 set_cursor(0, 3);   //next 3 prints >_< char by char
 send_data(0x3e);
 send_data(0x5f);
 send_data(0x3c);
 delay(500);
 clear_lcd();
 delay(500);
 set_cursor(1, 5);
 string(">_<");   //print string
 delay(500);
 clear_lcd();
 delay(500);
 set_cursor(0,1);
 clear_lcd();
 delay(1000);
 send_data(0);    //print first char of cgram
 delay(1000);
 set_cursor(1, 3);  //print second char of cgram
 send_data(1);
 delay(1000);
 set_cursor(0, 5);
 print_int(1234);
 delay(500);
 clear_lcd();
 delay(500);
 print_int(244, B);
 delay(500);
 clear_lcd();
 delay(500);
 set_cursor(1, 0);
 print_int(20, H);
 delay(500);
 clear_lcd();
 }

void string(String str){  //prints string 
  char* strng = (char*)malloc(sizeof(str));
  strcpy(strng, str.c_str());
  while(*strng){
    send_data(*strng++);
  }
}

void send_command(byte com){  //if command is sent, RS pin has to be low
  digitalWrite(RS, LOW);
  send_byte(com);
}

void send_data(byte data){   //if data is sent, RS pin has to be high 
  digitalWrite(RS, HIGH);
  send_byte(data);
}

void send_byte(byte b){   //in 4-bit mode, on the first pulse, send first the higher four bits, then the lowest
  digitalWrite(EN, HIGH);
  delay(0.5);
  for(int i = 0; i < 4; i++){
    digitalWrite(pins[i], b>>(4+i)&0x01);  //higher four bits first
  }
  digitalWrite(EN, LOW);
  delay(0.5);
  digitalWrite(EN, HIGH);
  delay(0.5);
  for(int i = 0; i < 4; i++){
    digitalWrite(pins[i], (b>>i)&0x01); //then the others
  }
  digitalWrite(EN, LOW);
  delay(0.5);
}

void lcd_init(){
  send_command(0x28);  //4-bit mode, 2 lines, 5x8matrix
  delay(5);
  send_command(0x0c); // d2=1->display on, d1=1->cursor on, d0=1->cursor blinking
  delay(5);
  send_command(0x06);  //d1=1->increment address counter after r/w; d0=0->display is shifted to the right
  delay(5);
  send_command(0x01); //clears display, returns cursor to home pos.
  delay(5);
}

void set_cursor(int row, int col){
  if(col > 16){
    col = col%16;   
  }
  if(row == 0){     
    send_command(0x80);  //at begining of first line
  }
  else if(row == 1){
    send_command(0xc0);   //at begining of second line
  }
  else{
    Serial.println("Invalid input, choose 0 or 1");
  }
  while(col>0){
      send_command(0x14);    //0x14 -> cursor goes right
      col--;
  }
}

void clear_lcd(){
  send_command(0x01);  //clears display, returns cursor to home
}


void draw_char(byte chars[8], byte adress){  //draws special char, saves it in cgram
  send_command(0x40|(adress<<3));  //0x40 -> for cgram
  for(int i = 0; i < 8; i++){
   send_data(chars[i]);      //save the bites in ram
   
  }
}


void print_int(int num) { //decimal
	if (num / 10 > 0) {    
		print_int(num / 10);  //go to first digit
	}
	send_data(num % 10 + 48);  //start to print them
}

void print_int(int num, unsigned char bh) {
	if (bh&H) {
		if (num / 16 > 0) {    //B is 0, H ->1
			Serial.println(num / 16);
			print_int(num / 16, H);
		}
		Serial.println(num % 16);
		send_data(num % 16 + 48);
		Serial.println("send");
	}
	else{
		if (num / 2 > 0) {
			print_int(num / 2, B);
		}
		send_data(num % 2 + 48);
	}
}
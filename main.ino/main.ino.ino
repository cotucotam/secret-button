void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // khởi tạo cổng serial xuất ra thông tin
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World"); // in ra dòng chữ Hello World
  delay(1000); // tạm dừng chương trình 1 giây
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
const int p1_buttons[] = {2, 3, 4, 5, 6}; // Player 1 Buttons (pins 2-6)
const int p2_buttons[] = {7, 8, 9, 10, 11}; // Player 2 Buttons (pins 7-11)
const int p1_leds[] = {4, 5, 6, 7, 8}; // Player 1 LEDs (pins 4-8)
const int p2_leds[] = {9, 10, 11, 12, 13}; // Player 2 LEDs (pins 9-13)

// LCD Definitions
LiquidCrystal_I2C lcd1(0x27, 16, 2); // LCD for Player 1 (I2C address 0x27)
LiquidCrystal_I2C lcd2(0x3F, 16, 2); // LCD for Player 2 (I2C address 0x3F)

// Game Variables
int p1_score = 0;
int p2_score = 0;
int action_speed = 2000;
int action_speed_min = 250;
int step_counter = 0;
bool game_started = false;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);

  // Initialize LCDs
  lcd1.begin();
  lcd2.begin();
  lcd1.setBacklight(1);
  lcd2.setBacklight(1);
  
  // Display startup message
  lcd1.print("Press button 1");
  lcd2.print("to start the game");

  // Initialize button pins
  for (int i = 0; i < 5; i++) {
    pinMode(p1_buttons[i], INPUT_PULLUP); // Set Player 1 buttons as input with pull-up
    pinMode(p2_buttons[i], INPUT_PULLUP); // Set Player 2 buttons as input with pull-up
    pinMode(p1_leds[i], OUTPUT); // Set Player 1 LEDs as output
    pinMode(p2_leds[i], OUTPUT); // Set Player 2 LEDs as output
  }

  // Initialize random seed
  randomSeed(analogRead(A0)); // Using unconnected analog pin for noise
}

void loop() {
  // Check if both players have pressed their start buttons
  if (!game_started) {
    if (digitalRead(p1_buttons[0]) == LOW && digitalRead(p2_buttons[0]) == LOW) {
      game_started = true;
      lcd1.clear();
      lcd2.clear();
      lcd1.print("Game Started!");
      lcd2.print("Game Started!");
      delay(1000); // Wait for a brief moment before starting the game
    }
    return; // Wait until both players press the button to start
  }

  // Game logic
  step_counter++;
  bool step_action = false;

  // Action speed update
  if (step_counter > action_speed) {
    step_counter = 0;
    step_action = true;
    action_speed = max(action_speed - round(action_speed / 50), action_speed_min);
    Serial.println(action_speed); // For debugging the action speed
  }

  // If it's time to trigger the next action
  if (step_action) {
    int pin_light = random(0, 5); // Randomly pick a LED to light up
    digitalWrite(p1_leds[pin_light], HIGH);
    digitalWrite(p2_leds[pin_light], HIGH);
    delay(500); // Keep the LEDs on for a short period
    digitalWrite(p1_leds[pin_light], LOW); // Turn off the LEDs after the delay
    digitalWrite(p2_leds[pin_light], LOW);
  }

  // Check Player 1's button presses
  for (int i = 0; i < 5; i++) {
    if (digitalRead(p1_buttons[i]) == LOW) {
      if (digitalRead(p1_leds[i]) == HIGH) {
        digitalWrite(p1_leds[i], LOW); // Turn off the LED if correct button is pressed
        p1_score++;
      }
    }
  }

  // Check Player 2's button presses
  for (int i = 0; i < 5; i++) {
    if (digitalRead(p2_buttons[i]) == LOW) {
      if (digitalRead(p2_leds[i]) == HIGH) {
        digitalWrite(p2_leds[i], LOW); // Turn off the LED if correct button is pressed
        p2_score++;
      }
    }
  }

  // Display Scores on the LCDs
  lcd1.setCursor(0, 0);
  lcd1.print("P1: ");
  lcd1.print(p1_score);
  
  lcd2.setCursor(0, 0);
  lcd2.print("P2: ");
  lcd2.print(p2_score);

  // Check if the game has ended
  if (p1_score >= 100 || p2_score >= 100) {
    lcd1.clear();
    lcd2.clear();
    if (p1_score > p2_score) {
      lcd1.print("P1 Wins!");
      lcd2.print("P2 Loses!");
    } else {
      lcd1.print("P1 Loses!");
      lcd2.print("P2 Wins!");
    }
    delay(3000); // Wait for 3 seconds before restarting or turning off
    resetGame();
  }
}

// Function to reset the game
void resetGame() {
  p1_score = 0;
  p2_score = 0;
  action_speed = 2000;
  game_started = false;
  lcd1.clear();
  lcd2.clear();
  lcd1.print("Press button 1");
  lcd2.print("to start the game");
}

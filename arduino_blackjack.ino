#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define POTENTIOMETER_PIN A0
#define BUTTON_PIN 4

int potentiometerValue;
int playerScore = 0;
int dealerScore = 0;
int unknownDealerCard = 0;
int buttonState = LOW;

bool dealerCardVisible = false;
bool playerMadeChoice = false;
bool dealerMadeChoice = false;
bool dealerIsBusted = false;

String messageToDisplay = "Hit or Stand!";
String scoreDisplay;
String playerChoice;
String currentPlayerTurn = "player"; 

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin();
  lcd.clear();
  delay(1000);
  randomSeed(analogRead(POTENTIOMETER_PIN));
  dealerScore = random(1, 10);
  playerScore = random(1, 10);
  unknownDealerCard = random(1, 10);
  delay(2000);
}

void loop() { 
  if(currentPlayerTurn == "player"){
    if(playerMadeChoice){
      if(playerChoice == "Stand"){ 
        dealerScore += unknownDealerCard;
        dealerCardVisible = true; 
        updateDisplay("You stood.");
        delay(3000);
        currentPlayerTurn = "dealer";
        updateDisplay("Dealer's turn..");
        delay(2000);
      }
      else if(playerChoice == "Busted"){ 
        dealerScore += unknownDealerCard;
        dealerCardVisible = true; 
        updateDisplay("You busted.");
        delay(3000);
        currentPlayerTurn = "dealer";
        updateDisplay("Dealer's turn..");
        delay(2000);
      }
      else if(playerChoice == "Hit"){
        playerScore += random(1, 10);
        if(playerScore > 21){
          playerChoice = "Busted";
        }
        else{
          updateDisplay("You hit.");
          delay(1500);
          playerMadeChoice = false;
        }
      }
    }else{
      potentiometerValue = analogRead(POTENTIOMETER_PIN);
      if(potentiometerValue < 512){ playerChoice = "Stand"; }
      else if(potentiometerValue > 512){ playerChoice = "Hit"; }
      updateDisplay("Pick:" + String(playerChoice));
      buttonState = digitalRead(BUTTON_PIN);
      if(buttonState == LOW){
        playerMadeChoice = true;
      }
    }
  }else if(currentPlayerTurn == "dealer"){
    if(dealerMadeChoice){
      if(playerChoice == "Hit"){
        if(!dealerIsBusted){ dealerScore += random(1,10); }
        if(dealerScore > 21){
          dealerIsBusted = true;
          if(playerScore <= 21 && playerScore < dealerScore){
            updateDisplay("You won.");
          }
          else if(playerScore > 21){
            updateDisplay("Both busted.");
          }
          else if(playerScore == dealerScore){
            updateDisplay("You tied.");
          }
          delay(2000);
          updateDisplay("Click the button.");
          delay(2000);
          buttonState = digitalRead(BUTTON_PIN);
          if(buttonState == LOW){
            dealerScore = 0;
            playerScore = 0;
            dealerCardVisible = false;
            updateDisplay(messageToDisplay);
            dealerScore = random(1, 10);
            playerScore = random(1, 10);
            unknownDealerCard = random(1, 10);
            playerMadeChoice = false;
            dealerMadeChoice = false;
            currentPlayerTurn = "player";
            dealerIsBusted = false;
            delay(2000);
          }
        }else{
          updateDisplay("Dealer hit.");
          delay(1500);
          dealerMadeChoice = false;
        }
      }
      else if(playerChoice == "Stand"){
        if(playerScore == dealerScore){
          updateDisplay("You tied.");
        }
        else if(playerScore <= 21){
          if(playerScore > dealerScore){
            updateDisplay("You won.");
          }
          else if(dealerScore > playerScore){
            updateDisplay("You busted.");
          }
        }
        else if(playerScore > 21){
          updateDisplay("You busted.");
        }
        delay(2000);
        updateDisplay("Click the button.");
        delay(2000);
        buttonState = digitalRead(BUTTON_PIN);
        if(buttonState == LOW){
          dealerScore = 0;
          playerScore = 0;
          dealerCardVisible = false;
          updateDisplay(messageToDisplay);
          dealerScore = random(1, 10);
          playerScore = random(1, 10);
          unknownDealerCard = random(1, 10);
          playerMadeChoice = false;
          dealerMadeChoice = false;
          currentPlayerTurn = "player";
          dealerIsBusted = false;
          delay(2000);
        }
      }
    }else{
      updateDisplay("It's thinking...");
      delay(1000);
      if(dealerScore >= 19){
        setDealersChoice("Stand");
      }
      else if(dealerScore < 19){
        if(dealerScore == playerScore){
          if(dealerScore < 19){
            setDealersChoice("Hit");
          }
          else if(dealerScore >= 19){
            setDealersChoice("Stand");
          } 
        }
        else if(playerScore >= 21){  
          if(playerScore > 21){
            setDealersChoice("Stand");
          }
          if(dealerScore < 19){
            setDealersChoice("Hit");
          }
          else if(dealerScore >= 19){
            setDealersChoice("Stand");
          }
        }
        else if(playerScore < 21){
          if(dealerScore < playerScore){
            setDealersChoice("Hit");
          }
          else if(dealerScore > playerScore){
            setDealersChoice("Stand");
          }
        }
      }
      delay(500);
    }
  }
}

void printLCD(String text, String text2){
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print(text);
  lcd.setCursor(0 ,1);
  lcd.print(text2);
  delay(200);
}

void setDealersChoice(String c){
  playerChoice = c;
  dealerMadeChoice = true;
}

void updateDisplay(String message){
  scoreDisplay = "You:" + String(playerScore) + "|Dealer:" + String(dealerScore) + String(dealerCardVisible ? "" : "*?");
  if(scoreDisplay.length() > 16) { 
    scoreDisplay = "You:" + String(playerScore) + "|D.:" + String(dealerScore) + String(dealerCardVisible ? "" : "*?" );
  }
  printLCD(message, scoreDisplay);
}
#pragma once

void menuSetup(); //setup menu

bool menuIsOpen();       //true if menu is open
bool menuItemIsActive(); //true if item is active

void menuSingleClick(); //can be called if encoder is clicked once
void menuDoubleClick(); //can be called if encoder is clicked twice
void menuTripleClick();
void menuUp();          //can be called if encoder value increased
void menuDown();        //can be called if encoder value decreased
char *getCurrentItem();
float getCurrentValue();

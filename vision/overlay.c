#include "overlay.h"

// Thresholds
const int lowerBattLimit = 30;
const int upperBattLimit = 50;

const float lowerPerfLimit = 50.0;
const float upperPerfLimit = 75.0;

const float lowerBrakeLimit = 40.0;
const float upperBrakeLimit = 70.0;

// Local functions
void renderText (char text[], int x, int y, int size, char foreground[], char background[]);
void renderTextAligned(char text[], int x, int y, int size, char foreground[], char background[], char horizontal, char vertical);
char *colourByValue(float value, float lower, float upper, char low[], char mid[], char high[]);

void renderRBatteryPercentage(int battPer);
void renderRSpeed(float speed, float gpsSpeed);
void renderRPerfPercentage(float perfPer);
void renderRDist(float dist);
void renderRBrakeTemp(float ftemp, float rtemp);
void renderRCadence(int cadence);
void renderRPower(int fpower, int rpower);
void renderRHR(int heartRate);
void renderRCO2(int CO2);

void renderFBatteryPercentage(int battPer);
void renderFSpeed(float speed, float gpsSpeed);
void renderFPerfPercentage(float perfPer);
void renderFDist(float dist);
void renderFBrakeTemp(float temperature);
void renderFCadence(int cadence);
void renderFPower(int power);
void renderFHR(int heartRate);

const float KM_TO_MI = 1.602; // Kilometers to miles

// Global variables
GRAPHICS_RESOURCE_HANDLE overlayImg; // Overlay
uint32_t widthOverlay, heightOverlay;

const int LAYER=6; // RPi camera preview is in layer 3, so this must be higher

// Colour constants (B,G,R,A)
char RED[] = {0x00,0x00,0xff,0xff};
char GREEN[] = {0x00,0xff,0x00,0xff};
char BLUE[] = {0xff,0x00,0x00,0xff};
char WHITE[] = {0xff,0xff,0xff,0xff};
char YELLOW[] = {0x00,0xff,0xff,0xff};
char CYAN[] = {0xff,0xff,0x00,0xff};
char MAGENTA[] = {0xff,0x00,0xff,0xff};

char GREY_BG[] = {0x00,0x00,0x00,0x80};
char CLEAR_BG[] = {0x00,0x00,0x00,0x00};

// Size 30 Text is 35 pixels tall. Size 50 is 56 pixels.

void updateOverlayFront(float spe, float dist, int pow, int cad, int hr, float perf, float fbrake, int batt, float gpsSpeed) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
      
   renderFSpeed(spe, gpsSpeed);
   renderFPerfPercentage(perf);
   renderFCadence(cad);
   renderFBatteryPercentage(batt);
   renderFPower(pow);
   renderFHR(hr);
   renderFDist(dist);
   renderFBrakeTemp(fbrake);
        
   graphics_update_displayed_resource(overlayImg, 0, 0, 0, 0);
}

void updateOverlayRear(float spe, float dist, int rpow, int fpow, int cad, int hr, float fbrake, float rbrake, int batt, float perf, int CO2, float gpsSpeed) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
      
   renderRSpeed(spe, gpsSpeed);
   renderRPerfPercentage(perf);
   renderRCadence(cad);
   renderRBatteryPercentage(batt);
   renderRPower(fpow, rpow);
   renderRHR(hr);
   
   renderRDist(dist);
   renderRBrakeTemp(fbrake, rbrake);
   renderRCO2(CO2);
        
   graphics_update_displayed_resource(overlayImg, 0, 0, 0, 0);
}






void renderFDist(float dist) {
   char temp[50];
   
   // Convert km travelled to miles remaining
   dist = 5.0 - (dist / KM_TO_MI);
   
   sprintf(temp, "DIST: %1.2f", dist);
   renderTextAligned(temp, 10, 45, 30, WHITE, GREY_BG,'l','t');
}

void renderFBrakeTemp(float temperature) {
   if (temperature < 60.0) return; // Do not render when under 60
   
   char temp[50];
   sprintf(temp, "BRAKE: %3.0f", temperature);
   renderTextAligned(temp, 10, 715, 30, RED, GREY_BG,'l','b');
}

void renderFBatteryPercentage(int battPer) {
   if (battPer > lowerBattLimit) return; // Do not render over lower limit
   
   char temp[50];
   sprintf(temp, "BATT: %3d", battPer);
   
   char *colourToUse = colourByValue(battPer, lowerBattLimit, upperBattLimit, RED, YELLOW, GREEN);
   
   renderTextAligned(temp, 1270, 715, 30, colourToUse, GREY_BG, 'r', 'b');
}

void renderFSpeed(float speed, float gpsSpeed) {
   char temp[50];
   sprintf(temp, "SPD/GSPD: %4.1f / %4.1f", speed/KM_TO_MI, gpsSpeed/KM_TO_MI);
   renderTextAligned(temp, 10, 10, 30, WHITE, GREY_BG,'l','t');
}

void renderFPerfPercentage(float perfPer) {
   char temp[50];
   sprintf(temp, "PERF: %5.1f", perfPer);

   char *colourToUse = colourByValue(perfPer, lowerPerfLimit, upperPerfLimit, RED, YELLOW, GREEN);

   renderTextAligned(temp, 10, 80, 30, colourToUse, GREY_BG, 'l', 't');
}

void renderFCadence(int cadence){
   char temp[30];
   sprintf(temp, "CAD: %3d", cadence);
   renderTextAligned(temp, 1270, 45, 30, WHITE, GREY_BG, 'r', 't');
}

void renderFPower(int power){
   char temp[30];
   sprintf(temp, "PWR: %3d", power);
   renderTextAligned(temp, 1270, 10, 30, WHITE, GREY_BG, 'r', 't');
}

void renderFHR(int heartRate){
   char temp[30];
   sprintf(temp, "HR: %3d", heartRate);
   renderTextAligned(temp, 1270, 80, 30, WHITE, GREY_BG, 'r', 't');
}













void renderRDist(float dist) {
   char temp[50];
   
   // Convert km travelled to miles remaining
   dist = 5.0 - (dist / KM_TO_MI);
   
   sprintf(temp, "DIST: %1.2f", dist);
   renderTextAligned(temp, 10, 65, 50, WHITE, GREY_BG,'l','t');
}

void renderRBrakeTemp(float ftemp, float rtemp) {

   char temp[50];
   sprintf(temp, "BRAKE: %3.0f/%3.0f (R/F)", rtemp, ftemp);
   
   char *colourToUse = colourByValue(rtemp, lowerBrakeLimit, upperBrakeLimit, GREEN, YELLOW, RED);
      
   renderTextAligned(temp, 10, 155, 30, colourToUse, GREY_BG,'l','t');
}

void renderRBatteryPercentage(int battPer) {
   char temp[50];
   sprintf(temp, "BATT: %3d", battPer);
   
   char *colourToUse = colourByValue(battPer, lowerBattLimit, upperBattLimit, RED, YELLOW, GREEN);
   
   renderTextAligned(temp, 10, 190, 30, colourToUse, GREY_BG, 'l', 't');
}

void renderRSpeed(float speed, float gpsSpeed) {
   char temp[50];
   sprintf(temp, "SPD/GSPD: %4.1f / %4.1f", speed/KM_TO_MI, gpsSpeed/KM_TO_MI);
   renderTextAligned(temp, 10, 10, 50, WHITE, GREY_BG,'l','t');
}

void renderRPerfPercentage(float perfPer) {
   char temp[50];
   sprintf(temp, "PERF: %5.1f", perfPer);

   char *colourToUse = colourByValue(perfPer, lowerPerfLimit, upperPerfLimit, RED, YELLOW, GREEN);

   renderTextAligned(temp, 10, 120, 30, colourToUse, GREY_BG, 'l', 't');
}

void renderRCadence(int cadence){
   char temp[30];
   sprintf(temp, "CAD: %3d", cadence);
   renderTextAligned(temp, 1270, 65, 30, WHITE, GREY_BG, 'r', 't');
}

void renderRPower(int fpower, int rpower){
   char temp[30];
   sprintf(temp, "PWR: %3d/%3d", rpower, (fpower + rpower));
   renderTextAligned(temp, 1270, 10, 50, WHITE, GREY_BG, 'r', 't');
}

void renderRHR(int heartRate){
   char temp[30];
   sprintf(temp, "HR: %3d", heartRate);
   renderTextAligned(temp, 1270, 100, 30, WHITE, GREY_BG, 'r', 't');
}

void renderRCO2(int CO2) {
   char temp[50];
   sprintf(temp, "CO2: %4d", CO2);
   renderTextAligned(temp, 1270, 135, 30, WHITE, GREY_BG,'r','t');
}





char *colourByValue(float value, float lower, float upper, char low[], char mid[], char high[]) {
   // Returns a colour based on how the value compares to two limits
   char *resultColour;
   
   if (value < lower) resultColour = &low[0];
   else if (value > upper) resultColour = &high[0];
   else resultColour = &mid[0];
   
   return resultColour;
}


void startOverlay(bool cameraOn) {
   
   if (cameraOn) system("python ./camera.py &"); // Start camera script
   
   bcm_host_init();
   int s;

   if (get_processor_id() == PROCESSOR_BCM2838)
   {
      puts("This application is not available on the Pi4\n\n");
      exit(0);
   }

   s = gx_graphics_init(".");
   assert(s == 0);
   
   s = graphics_get_display_size(0, &widthOverlay, &heightOverlay);
   assert(s == 0);

   s = gx_create_window(0, widthOverlay, heightOverlay, GRAPHICS_RESOURCE_RGBA32, &overlayImg);
   assert(s == 0);

   // transparent before display to avoid screen flash
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));

   graphics_display_resource(overlayImg, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);
}

void renderText (char text[], int x, int y, int size, char foreground[], char background[]) {
      graphics_resource_render_text_ext(overlayImg, x, y,
                                     GRAPHICS_RESOURCE_WIDTH,
                                     GRAPHICS_RESOURCE_HEIGHT,
                                     GRAPHICS_RGBA32(foreground[0],foreground[1],foreground[2],foreground[3]), /* fg */
                                     GRAPHICS_RGBA32(background[0],background[1],background[2],background[3]), /* bg */
                                     text, strlen(text), size);
}

void renderTextAligned(char text[], int x, int y, int size, char foreground[], char background[], char horizontal, char vertical) {
   uint32_t widthOfText=0, heightOfText=0; // Used to store the rendered dimensions of text
   graphics_resource_text_dimensions_ext(overlayImg, text, strlen(text), &widthOfText, &heightOfText, size);

   // Horizontal Justification
   if ((horizontal == 'c') || (horizontal == 'C')) x = x - (widthOfText / 2);
   else if ((horizontal == 'r') || (horizontal == 'R')) x = x - widthOfText;
   // Otherwise assumes left
   
   // Vertical Justification
   if ((vertical == 'c') || (vertical == 'C')) y = y - (heightOfText / 2);
   else if ((vertical == 'b') || (vertical == 'B')) y = y - heightOfText;
   // Otherwise assumes the top
   //printf("TEXT SIZE: %d, HEIGHT: %d",size, heightOfText);
   renderText (text, x, y, size, foreground, background);
}

void closeOverlay() {
   graphics_display_resource(overlayImg, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 0);
   graphics_delete_resource(overlayImg);
   system("sudo pkill python"); // Kills the preview process (and any other python processes)
}

#include "overlay.h"

// Local functions
void renderText (char text[], int x, int y, int size, char foreground[], char background[]);
void renderTextAligned(char text[], int x, int y, int size, char foreground[], char background[], char horizontal, char vertical);

void renderRBatteryPercentage(int battPer);
void renderRSpeed(float speed);
void renderRPerfPercentage(float perfPer);
void renderRDist(float dist);
void renderRBrakeTemp(float ftemp, float rtemp);
void renderRCadence(int cadence);
void renderRPower(int fpower, int rpower);
void renderRHR(int heartRate);
void renderRCO2(int CO2);

void renderFBatteryPercentage(int battPer);
void renderFSpeed(float speed);
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

// Colour constants
char RED[] = {0xff,0x00,0x00,0xff};
char GREEN[] = {0x00,0xff,0x00,0xff};
char BLUE[] = {0x00,0x00,0xff,0xff};
char WHITE[] = {0xff,0xff,0xff,0xff};
char YELLOW[] = {0x00,0xff,0xff,0xff};
char CYAN[] = {0xff,0xff,0x00,0xff};
char MAGENTA[] = {0xff,0x00,0xff,0xff};

char GREY_BG[] = {0x00,0x00,0x00,0x80};
char CLEAR_BG[] = {0x00,0x00,0x00,0x00};

// Thresholds, should move to a config file
const int lowerBattLimit = 30;
const int upperBattLimit = 50;

const float lowerPerfLimit = 98.0;
const float upperPerfLimit = 102.0;

// Size 30 Text is 35 pixels tall. Size 50 is 56 pixels.

void updateOverlayFront(float spe, float dist, int pow, int cad, int hr, float perf, float fbrake, int batt) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
      
   renderFSpeed(spe);
   renderFPerfPercentage(perf);
   renderFCadence(cad);
   renderFBatteryPercentage(batt);
   renderFPower(pow);
   renderFHR(hr);
   renderFDist(dist);
   renderFBrakeTemp(fbrake);
        
   graphics_update_displayed_resource(overlayImg, 0, 0, 0, 0);
}

void updateOverlayRear(float spe, float dist, int rpow, int fpow, int cad, int hr, float fbrake, float rbrake, int batt, float perf, int CO2) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
      
   renderRSpeed(spe);
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
   sprintf(temp, "Distance to end: %1.2f M", dist / KM_TO_MI);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
}

void renderFBrakeTemp(float temperature) {
   if (temperature < 60.0) return; // Do not render when under 60
   
   char temp[50];
   sprintf(temp, "BRAKE TEMP: %3.2f*C", temperature);
   renderTextAligned(temp, 10, 715, 50, RED, GREY_BG,'l','b');
}

void renderFBatteryPercentage(int battPer) {
   char temp[50];
   sprintf(temp, "Battery: %3d%%", battPer);
   
   if (battPer < lowerBattLimit) renderTextAligned(temp, 1270, 10, 30, RED, GREY_BG, 'r', 't');
   else if (battPer > upperBattLimit) renderTextAligned(temp, 1270, 10, 30, GREEN, GREY_BG, 'r', 't');
   else renderTextAligned(temp, 1270, 10, 30, YELLOW, GREY_BG, 'r', 't');
}

void renderFSpeed(float speed) {
   char temp[50];
   sprintf(temp, "Speed: %5.2f MPH", speed/KM_TO_MI);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
}


void renderFPerfPercentage(float perfPer) {
   char temp[50];
   sprintf(temp, "Performance: %3.1f%%", perfPer);

   if (perfPer < lowerPerfLimit) renderTextAligned(temp, 10, 710, 30, RED, GREY_BG, 'l', 'b');
   else if (perfPer > upperPerfLimit) renderTextAligned(temp, 10, 710, 30, GREEN, GREY_BG, 'l', 'b');
   else renderTextAligned(temp, 10, 654, 30, YELLOW, GREY_BG, 'l', 'b');
}

void renderFCadence(int cadence){
   char temp[30];
   sprintf(temp, "%3d", cadence);
   
   const int bottom = 640;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("RPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Cadence:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderFPower(int power){
   char temp[30];
   sprintf(temp, "     %3d", power);
   
   const int bottom = 675;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("W", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Power:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderFHR(int heartRate){
   char temp[30];
   sprintf(temp, "%3d", heartRate);
   
   const int bottom = 710;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("BPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("HR:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}













void renderRDist(float dist) {
   char temp[50];
   sprintf(temp, "Distance to end: %1.2f M", dist / KM_TO_MI);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
}

void renderRBrakeTemp(float ftemp, float rtemp) {

   char temp[50];
   sprintf(temp, "BRAKE TEMP: %3.2f/%3.2f *C (F/R)", ftemp, rtemp);
   renderTextAligned(temp, 10, 715, 50, RED, GREY_BG,'l','b');
}

void renderRBatteryPercentage(int battPer) {
   char temp[50];
   sprintf(temp, "Battery: %3d%%", battPer);
   
   if (battPer < lowerBattLimit) renderTextAligned(temp, 1270, 10, 30, RED, GREY_BG, 'r', 't');
   else if (battPer > upperBattLimit) renderTextAligned(temp, 1270, 10, 30, GREEN, GREY_BG, 'r', 't');
   else renderTextAligned(temp, 1270, 10, 30, YELLOW, GREY_BG, 'r', 't');
}

void renderRSpeed(float speed) {
   char temp[50];
   sprintf(temp, "Speed: %5.2f MPH", speed/KM_TO_MI);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
}


void renderRPerfPercentage(float perfPer) {
   char temp[50];
   sprintf(temp, "Performance: %3.1f%%", perfPer);

   if (perfPer < lowerPerfLimit) renderTextAligned(temp, 10, 710, 30, RED, GREY_BG, 'l', 'b');
   else if (perfPer > upperPerfLimit) renderTextAligned(temp, 10, 710, 30, GREEN, GREY_BG, 'l', 'b');
   else renderTextAligned(temp, 10, 654, 30, YELLOW, GREY_BG, 'l', 'b');
}

void renderRCadence(int cadence){
   char temp[30];
   sprintf(temp, "%3d", cadence);
   
   const int bottom = 640;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("RPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Cadence:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderRPower(int fpower, int rpower){
   char temp[30];
   sprintf(temp, " %3d/%3d", rpower, (fpower + rpower));
   
   const int bottom = 675;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("W", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Power:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderRHR(int heartRate){
   char temp[30];
   sprintf(temp, "%3d", heartRate);
   
   const int bottom = 710;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("BPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("HR:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderRCO2(int CO2) {
   char temp[50];
   sprintf(temp, "CO2: %4d ppm", CO2);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
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

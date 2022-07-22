#include "overlay.h"

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
const float lowerBattLimit = 25;
const float upperBattLimit = 50;

const float lowerPerfLimit = 98;
const float upperPerfLimit = 102;

// Size 30 Text is 35 pixels tall. Size 50 is 56 pixels.

void updateOverlayTest(int iteration, int pow, int cad, int hr) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
 
      
   renderSpeed(45);
   renderPerfPercentage(100);
   renderCadence(cad);
   renderBatteryPercentage(iteration);
   renderPower(pow);
   renderHR(hr);
   renderTemp(30);
   renderHumidity(50);
        
   graphics_update_displayed_resource(overlayImg, 0, 0, 0, 0);
}

void updateOverlay(float spe, float dist, int pow, int cad, int hr, float temper, float humid, int batt) {
   graphics_resource_fill(overlayImg, 0, 0, widthOverlay, heightOverlay, GRAPHICS_RGBA32(0,0,0,0x00));
      
   renderSpeed(spe);
   renderPerfPercentage(100);
   renderCadence(cad);
   renderBatteryPercentage(batt);
   renderPower(pow);
   renderHR(hr);
   renderTemp(temper);
   renderHumidity(humid);
        
   graphics_update_displayed_resource(overlayImg, 0, 0, 0, 0);
}

void renderBatteryPercentage(int battPer) {
   char temp[50];
   sprintf(temp, "Battery: %3d%%", battPer);
   
   if (battPer < lowerBattLimit) renderTextAligned(temp, 1270, 10, 30, RED, GREY_BG, 'r', 't');
   else if (battPer > upperBattLimit) renderTextAligned(temp, 1270, 10, 30, GREEN, GREY_BG, 'r', 't');
   else renderTextAligned(temp, 1270, 10, 30, YELLOW, GREY_BG, 'r', 't');
}

void renderSpeed(float speed) {
   char temp[50];
   sprintf(temp, "Speed: %5.2f km/h / %5.2f MPH", speed, speed/KM_TO_MI);
   renderTextAligned(temp, 10, 715, 50, WHITE, GREY_BG,'l','b');
}


void renderPerfPercentage(float perfPer) {
   char temp[50];
   sprintf(temp, "Performance: %3.1f%%", perfPer);

   if (perfPer < lowerPerfLimit) renderTextAligned(temp, 10, 710, 30, RED, GREY_BG, 'l', 'b');
   else if (perfPer > upperPerfLimit) renderTextAligned(temp, 10, 710, 30, GREEN, GREY_BG, 'l', 'b');
   else renderTextAligned(temp, 10, 654, 30, YELLOW, GREY_BG, 'l', 'b');
}

void renderCadence(int cadence){
   char temp[30];
   sprintf(temp, "%3d", cadence);
   
   const int bottom = 640;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("RPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Cadence:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderPower(int power){
   char temp[30];
   sprintf(temp, "     %3d", power);
   
   const int bottom = 675;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("W", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("Power:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderHR(int heartRate){
   char temp[30];
   sprintf(temp, "%3d", heartRate);
   
   const int bottom = 710;
   renderTextAligned("                     ", 1270, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("BPM", 1195, bottom, 30, WHITE, GREY_BG, 'l', 'b');
   renderTextAligned(temp, 1185, bottom, 30, WHITE, GREY_BG, 'r', 'b');
   renderTextAligned("HR:", 1100, bottom, 30, WHITE, GREY_BG, 'r', 'b');
}

void renderTemp(float temperature){
   char temp[30];
   sprintf(temp, "Temp.: %3.1fC", temperature);
   renderTextAligned(temp, 10, 10, 30, GREEN, GREY_BG, 'l', 't');
}
void renderHumidity(float humidity){
   char temp[30];
   sprintf(temp, "R.H.: %3.1f%%", humidity);
   renderTextAligned(temp, 10, 45, 30, GREEN, GREY_BG, 'l', 't');
}
        

void startOverlay() {
   system("python ./camera.py &"); // Start camera script
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

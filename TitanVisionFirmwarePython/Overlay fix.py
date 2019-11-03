mCurrentOverlayLayer = 3 # as a variable in the Osd object

####


theTmpOverlay = self.mCamera.add_overlay(anOverlayImg.tobytes(), size=anOverlayImg.size, layer=self.mCurrentOverlayLayer)

self.mCurrentOverlayLayer = self.mCurrentOverlayLayer + 1
if self.mCurrentOverlayLayer > 4:
self.mCurrentOverlayLayer = 3
		
if self.mOverlay != None:
self.mCamera.remove_overlay(self.mOverlay)
	
self.mOverlay = theTmpOverlay
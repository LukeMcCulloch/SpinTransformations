@echo off

echo ======================= SpinXForm =======================
echo To specify inputs, edit this file (SpinXForm.bat) using
echo your favorite text editor.  The first input should be a
echo mesh in Wavefront OBJ format; the second input should be
echo a grayscale image in Truevision TGA format.
echo =========================================================

spinxform.exe examples/bumpy/sphere.obj examples/bumpy/bumpy.tga

pause


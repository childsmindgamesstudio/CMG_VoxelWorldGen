@echo off
echo Adding remote origin...
git remote add origin https://github.com/childsmindgamesstudio/CMG_VoxelWorldGen.git

echo Setting main branch...
git branch -M main

echo Pushing to GitHub...
git push -u origin main

echo Done!
pause
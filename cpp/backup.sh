read -p "Commit: " COMMIT

./buildjs.bat
cd ..
git add *
git commit -am "$COMMIT"
git push
cd cpp
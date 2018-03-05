read -p "Commit: " COMMIT

cd ..
git add *
git commit -am "$COMMIT"
git push
cd cpp
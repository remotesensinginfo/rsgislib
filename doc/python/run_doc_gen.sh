pip install python-docs-theme
if [ ! -d "./html" ]; then
  mkdir ./html
fi
sphinx-build -b html -a ./source/ ./html/

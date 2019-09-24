pip install python-docs-theme
pip install sphinxcontrib.yt
if [ ! -d "./html" ]; then
  mkdir ./html
fi
sphinx-build -b html -a ./source/ ./html/

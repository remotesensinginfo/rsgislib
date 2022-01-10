#docker run -itv ${PWD}:/data petebunting/au-eoed-dev /bin/bash
#export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/pete/Temp/rsgislib_v5_dev_install/lib
#export PYTHONPATH=$PYTHONPATH:/Users/pete/Temp/rsgislib_v5_dev_install/lib/python3.9/site-packages

#pip install python-docs-theme
pip install sphinx-rtd-theme
pip install sphinxcontrib.yt
if [ ! -d "./html" ]; then
  mkdir ./html
fi
sphinx-build -b html -a ./source/ ./html/

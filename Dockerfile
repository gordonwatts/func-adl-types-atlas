# Build the development container

FROM atlas/analysisbase:22.2.106
# 22.2.106 - for R22
# 21.2.184 - for R21


RUN echo "source ~/release_setup.sh" >> ~/.bashrc

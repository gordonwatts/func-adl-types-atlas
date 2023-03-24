# Build the development container

FROM atlas/analysisbase:22.2.106
# 22.2.106 - for R22
# 21.2.184 - for R21
# Repo for atlas container releases: https://gitlab.cern.ch/atlas/athena/container_registry/8439


RUN echo "source ~/release_setup.sh" >> ~/.bashrc

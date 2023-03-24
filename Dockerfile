# Build the development container

FROM gitlab-registry.cern.ch/atlas/athena/analysisbase:21.2.247
# 22.2.113 - for R22
# 21.2.184 - for R21
# Repo for atlas container releases: https://gitlab.cern.ch/atlas/athena/container_registry/8439


RUN echo "source ~/release_setup.sh" >> ~/.bashrc

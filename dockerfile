FROM ubuntu:22.04
SHELL ["/bin/bash", "-c"]
COPY BUILD/PACKAGE/CORE/ICE-CORE-2.0.tar.gz /ICE-CORE-2.0.tar.gz
COPY BUILD/PACKAGE/CORE/install-core.sh /install-core.sh
RUN apt-get update
RUN apt-get install nodejs -y && apt-get install libssl3 -y
RUN mkdir /data && chmod 777 /data
RUN chmod 777 /install-core.sh
RUN /install-core.sh
RUN cd /data/ICE2/LUTS/ && cp panelV2.lut panelV1.lut && cp PAR-V1.calib parV1.calib && cp PAR-V2.calib parV2.calib && cp MH-V1.calib mhV1.calib && cp MH-V2.calib mhV2.calib && cp BL-V1.calib backV1.calib && cp BL-V1.calib backV2.calib
RUN apt-get install gdb -y
ENTRYPOINT ["tail", "-f", "/dev/null"]
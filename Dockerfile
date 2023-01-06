FROM wiiuenv/devkitppc:20221228

COPY --from=wiiuenv/libfunctionpatcher:20230106 /artifacts $DEVKITPRO
COPY --from=wiiuenv/wiiumodulesystem:20230106 /artifacts $DEVKITPRO

WORKDIR project

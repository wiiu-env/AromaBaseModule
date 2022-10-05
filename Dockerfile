FROM wiiuenv/devkitppc:20220917

COPY --from=wiiuenv/libfunctionpatcher:20220904 /artifacts $DEVKITPRO
COPY --from=wiiuenv/wiiumodulesystem:20221005 /artifacts $DEVKITPRO

WORKDIR project

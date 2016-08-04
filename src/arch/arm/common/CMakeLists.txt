phabos_add_srcs(
  delay.c
  lowio.c)

message("")
phabos_cond_compile(CONFIG_ARM_SEMIHOSTING semihosting.c)
message("")

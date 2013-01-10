
uint32_t* read_memory(
    max_maxfile_t *const maxfile, max_device_handle_t *const device,
    const max_fpga_t fpga,
    unsigned offset_bytes, unsigned data_length_bytes);

int memory_write(
    max_maxfile_t *const maxfile, max_device_handle_t *const device,
    const max_fpga_t fpga, uint32_t* data,
    unsigned offset_bytes, unsigned data_length_bytes, int pattern);

int setup_address_generator_body(
    max_maxfile_t *const maxfile, max_device_handle_t *const device,
    const max_fpga_t fpga, struct max_memory_setting *const ctx,
    char *stream_name, char *cmd_name,
    unsigned offset_bytes, unsigned data_length_bytes, int interrupt, int pattern);

int setup_address_generator(
    max_maxfile_t *const maxfile, max_device_handle_t *const device,
    const max_fpga_t fpga, struct max_memory_setting *const ctx,
    char *stream_name,
    unsigned offset_bytes, unsigned data_length_bytes, int interrupt, int pattern);

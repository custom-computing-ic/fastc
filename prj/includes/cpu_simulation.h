// Max
void max_configure();
// Stencil
void construct_stencil();
// Propagation
void prop_source(int);
// Take time derivative of data and mute out direct arrival
void differentiate_mute_data(char *);
// Migrate a single shot
void migrate_shot();


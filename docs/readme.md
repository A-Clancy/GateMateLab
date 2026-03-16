# GateMate Lab Documentation

This folder contains supporting documentation for the GateMate Lab
implementation.

The material here complements the main repository README and provides
additional detail on the development environment, entity structure, and
validation approach used during the project.

The aim of this documentation is to make the system easy to understand
and reproduce without introducing unnecessary complexity.

------------------------------------------------------------------------

## Documents

### environment.md

Defines the minimal Home Assistant environment required to reproduce the
GateMate Lab instance. This includes required add-ons, integrations, and
development workflow assumptions.

### naming_convention.md

Documents the entity naming conventions used throughout the system and
lists the core entities used by GateMate.

Consistent naming is important in Home Assistant projects where many
entities exist in a shared namespace.

### validation/

Contains documentation describing the validation approach used during
system testing.

These tests demonstrate that:

-   BLE transport operates correctly
-   LoRa relay transport operates correctly
-   transport switching works without automation changes
-   vehicle detection logic behaves as expected
-   fail-safe behaviour works under sensor dropout conditions

------------------------------------------------------------------------

## Scope

This documentation focuses only on the components relevant to the
GateMate system.

General Home Assistant configuration and unrelated integrations are
intentionally excluded to keep the project focused and reproducible.

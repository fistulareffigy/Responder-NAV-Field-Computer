Import("env")

from pathlib import Path


def patch_i2c_slave():
    pkg_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
    if not pkg_dir:
        print("patch_i2c_slave: framework package not found")
        return
    target = Path(pkg_dir) / "cores" / "esp32" / "esp32-hal-i2c-slave.c"
    if not target.exists():
        print(f"patch_i2c_slave: missing {target}")
        return

    text = target.read_text(encoding="utf-8", errors="ignore")
    old = (
        "#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)\n"
        "#if defined(CONFIG_IDF_TARGET_ESP32P4)\n"
        "  i2c_ll_slave_init(i2c->dev);\n"
        "  i2c_ll_enable_fifo_mode(i2c->dev, true);\n"
        "#else\n"
        "  i2c_ll_set_mode(i2c->dev, I2C_BUS_MODE_SLAVE);\n"
        "  i2c_ll_enable_pins_open_drain(i2c->dev, true);\n"
        "  i2c_ll_enable_fifo_mode(i2c->dev, true);\n"
        "#endif\n"
        "#else\n"
        "  i2c_ll_slave_init(i2c->dev);\n"
        "  i2c_ll_slave_set_fifo_mode(i2c->dev, true);\n"
        "#endif\n"
    )
    new = (
        "#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)\n"
        "  i2c_ll_set_mode(i2c->dev, I2C_BUS_MODE_SLAVE);\n"
        "  i2c_ll_enable_pins_open_drain(i2c->dev, true);\n"
        "  i2c_ll_enable_fifo_mode(i2c->dev, true);\n"
        "#else\n"
        "  i2c_ll_slave_init(i2c->dev);\n"
        "  i2c_ll_slave_set_fifo_mode(i2c->dev, true);\n"
        "#endif\n"
    )

    if new in text:
        print("patch_i2c_slave: already patched")
        return
    if old not in text:
        print("patch_i2c_slave: target block not found; no changes applied")
        return

    target.write_text(text.replace(old, new), encoding="utf-8")
    print("patch_i2c_slave: applied")


patch_i2c_slave()

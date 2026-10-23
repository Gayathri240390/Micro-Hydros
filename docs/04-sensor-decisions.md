# Sensor Decisions

## Criteria

DHT11 and DHT22 are prohibited. Key criteria are adequate accuracy for environmental trends, suitability for humid conditions, ESP32-compatible interfaces, integration effort, cost/availability, and protection appropriate to the enclosure.

## Provisional recommendation

| Measurement | Candidate | Rationale | Caveat |
|---|---|---|---|
| Inside air temperature and relative humidity | Sensirion SHT40 family, preferably an appropriate protected variant | One digital I2C component supplies both values; the family offers suitable measurement ranges and accuracy for a prototype. | Avoid condensation/direct water; confirm exact breakout voltage, pinout, variant, and datasheet limits. |
| Outside air temperature | Second SHT40, or a simpler digital temperature sensor after comparison | Reusing SHT40 reduces driver and integration variety. | Use a shaded, ventilated enclosure; protection can affect response. |
| Nutrient-water temperature | DS18B20 in a supplier-specified sealed probe | Digital 1-Wire and commonly available probe assemblies suit a first prototype. | The IC itself is not a waterproof probe. Verify authenticity, seal, cable entry, and wetted materials. |

This is a reasoned proposal, not measured proof of accuracy or lifetime. Confirm exact manufacturer datasheets and purchased part numbers; breakout boards and probe assemblies may not match IC specifications or quality.

## Alternatives

- **SHT31:** established I2C temperature/RH option; compare exact specifications and availability with SHT40.
- **BME280:** adds pressure, which is not required; check its temperature/RH specifications and thermal placement.
- **TMP117:** precise temperature option but does not measure humidity and does not eliminate the need for a water probe.

## Integration and calibration plan

Follow the purchased SHT40 breakout's voltage and I2C requirements. Use appropriate pull-ups and wiring for the bus length. Place the inside sensor away from electronics, LED drivers, pump heat, and droplets; shade and ventilate the outside sensor; keep water-sensor electronics outside the reservoir.

Before claiming accuracy, compare each assembled channel with a suitable reference over representative conditions, record offset and uncertainty, then repeat after installation. A comparison table is not a calibration result.
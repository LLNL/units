import pandas as pd

# Load the XLS file
file_path = "list-one.xls"
xls_data = pd.read_excel(file_path, sheet_name="Active")

outfile = open("currencydata.txt", "w")
for index, row in xls_data.iterrows():
    if index >= 3:
        currency = row.iloc[1]
        if currency in ["US Dollar", "Euro"]:
            continue
        if len(currency) > 39:
            continue
        currency = currency.replace(" ", "")
        line1 = (
            f'{{"{currency}",{{1.0, precise::currency, generateCurrencyCode("{row.iloc[2]}")}}}},'
        )
        outfile.write(f"{line1}\n")

outfile.write("\n\n\n")
outfile.write(
    "UNITS_CPP14_CONSTEXPR_OBJECT std::array<std::pair<const char*, std::uint32_t>, 1213> defined_commodity_codes{\n"
)

for index, row in xls_data.iterrows():
    if index >= 3:
        currency = row.iloc[1]
        if currency in ["US Dollar", "Euro"]:
            continue
        outfile.write(f'{{"{row.iloc[2]}",generateCurrencyCode("{row.iloc[2]}")}},\n')
        outfile.write(f'{{"{row.iloc[3]}",generateCurrencyCode("{row.iloc[2]}")}},\n')

outfile.write("};\n\n")

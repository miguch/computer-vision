from xlsxwriter.workbook import Workbook
import sys
import csv

if len(sys.argv) < 2:
    print('Error: No file specified')
else:
    extensionIndex = sys.argv[1].rfind('.')
    xlsxName = sys.argv[1][:extensionIndex] + ".xlsx"
    workbook = Workbook(xlsxName)
    worksheet = workbook.add_worksheet()
    with open(sys.argv[1], 'rt', encoding='utf8') as f:
        reader = csv.reader(f)
        for r, row in enumerate(reader):
            for c, col in enumerate(row):
                worksheet.write(r, c, col)
    workbook.close()

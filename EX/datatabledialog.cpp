// #include "datatabledialog.h"

// DataTableDialog::DataTableDialog(QWidget *parent) :
//     QDialog(parent) {
//     setWindowTitle("VL1 and VL2 Data Tables");
//     resize(800, 400);  // Resize the dialog

//     // Initialize the table_1
//     table_1 = new QTableWidget(8, 8, this); // 8 rows and 8 columns
//     table_1->setHorizontalHeaderLabels(QStringList() << "Col1" << "Col2" << "Col3" << "Col4" << "Col5" << "Col6" << "Col7" << "Col8"); // Set header labels
//     for(int i = 0; i < 8; i++){
//         table_1->setColumnWidth(i, 50);
//     }

//     // Initialize the table_2
//     table_2 = new QTableWidget(8, 8, this); // 8 rows and 8 columns
//     table_2->setHorizontalHeaderLabels(QStringList() << "Col1" << "Col2" << "Col3" << "Col4" << "Col5" << "Col6" << "Col7" << "Col8"); // Set header labels
//     for(int i = 0; i < 8; i++){
//         table_2->setColumnWidth(i, 50);
//     }

//     // Create a vertical layout and add the table
//     QVBoxLayout *layout = new QVBoxLayout(this);
//     layout->addWidget(table_1);
//     layout->addWidget(table_2);
//     setLayout(layout); // Set the layout to the dialog
// }

// DataTableDialog::~DataTableDialog() {
//     // No need to delete ui, as it's not used
// }

// void DataTableDialog::clearTable() {
//     table_1->clear(); // Clears the entire table
//     table_1->setRowCount(8); // Reset the row count
//     table_1->setColumnCount(8); // Reset the column count

//     table_2->clear(); // Clears the entire table
//     table_2->setRowCount(8); // Reset the row count
//     table_2->setColumnCount(8); // Reset the column count
// }

// void DataTableDialog::updateTable_1(int row, int col, int value) {
//     if (row >= 0 && row < table_1->rowCount() && col >= 0 && col < table_1->columnCount()) {
//         table_1->setItem(row, col, new QTableWidgetItem(QString::number(value))); // Update the specific cell with the value
//     }
// }

// void DataTableDialog::updateTable_2(int row, int col, int value) {
//     if (row >= 0 && row < table_2->rowCount() && col >= 0 && col < table_2->columnCount()) {
//         table_2->setItem(row, col, new QTableWidgetItem(QString::number(value))); // Update the specific cell with the value
//     }
// }


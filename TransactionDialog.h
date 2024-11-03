#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include <QDialog>
#include "ShadowWidget.h"
namespace Ui {
  class TransactionDialog;
}

class TransactionDialog : public ShadowWidget<QDialog>
{
  Q_OBJECT

public:
  explicit TransactionDialog(QWidget *parent = nullptr);
  ~TransactionDialog();

Q_SIGNALS:
  void deposit(int amount,  const QString&);
  void withdraw(int amount);
  void transfer(int amount, const QString& to);
private:
  Ui::TransactionDialog *ui;
};

#endif // TRANSACTIONDIALOG_H

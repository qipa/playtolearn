
#pragma once

#include <vector>

#include <QObject>

#include "ui/TransitionNodePolicy.hpp"

namespace PlayToLearn {
namespace UI {

class TransitionNodeCreator : public QObject {
  Q_OBJECT
public:
  TransitionNodeCreator();
  size_t size() const;
  std::string node_name(int node_number) const;
  std::string state_name() const;

public slots:
  void create_node(int node_number);
  void create_state();

signals:
  void node_created(TransitionNodePolicy* policy);
  void state_created();

private:
  std::vector<TransitionNodePolicy*> policies_;
};


}  /* namespace UI */
}  /* namespace PlayToLearn */
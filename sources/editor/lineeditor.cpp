/*
	Copyright 2006-2020 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "lineeditor.h"
#include "styleeditor.h"
#include "partline.h"
#include "qet.h"
#include "qeticons.h"
#include "QPropertyUndoCommand/qpropertyundocommand.h"
#include "elementscene.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param line La ligne a editer
	@param parent le Widget parent
*/
LineEditor::LineEditor(QETElementEditor *editor, PartLine *line, QWidget *parent) :
	ElementItemEditor(editor, parent),
	part(line),
	m_locked (false)
{
	style_ = new StyleEditor(editor);
	
	x1 = new QDoubleSpinBox();
	y1 = new QDoubleSpinBox();
	x2 = new QDoubleSpinBox();
	y2 = new QDoubleSpinBox();
	
	x1 -> setRange(-5000, 5000);
	y1 -> setRange(-5000, 5000);
	x2 -> setRange(-5000, 5000);
	y2 -> setRange(-5000, 5000);
	
	end1_type = new QComboBox();
	end1_type -> addItem(QET::Icons::EndLineNone,     tr("Normale",                "type of the 1st end of a line"), Qet::None    );
	end1_type -> addItem(QET::Icons::EndLineSimple,   tr("Flèche simple",       "type of the 1st end of a line"), Qet::Simple  );
	end1_type -> addItem(QET::Icons::EndLineTriangle, tr("Flèche triangulaire", "type of the 1st end of a line"), Qet::Triangle);
	end1_type -> addItem(QET::Icons::EndLineCircle,   tr("Cercle",                 "type of the 1st end of a line"), Qet::Circle  );
	end1_type -> addItem(QET::Icons::EndLineDiamond,  tr("Carré",               "type of the 1st end of a line"), Qet::Diamond );
	end2_type = new QComboBox();
	end2_type -> addItem(QET::Icons::EndLineNone,     tr("Normale",                "type of the 2nd end of a line"), Qet::None    );
	end2_type -> addItem(QET::Icons::EndLineSimple,   tr("Flèche simple",       "type of the 2nd end of a line"), Qet::Simple  );
	end2_type -> addItem(QET::Icons::EndLineTriangle, tr("Flèche triangulaire", "type of the 2nd end of a line"), Qet::Triangle);
	end2_type -> addItem(QET::Icons::EndLineCircle,   tr("Cercle",                 "type of the 2nd end of a line"), Qet::Circle  );
	end2_type -> addItem(QET::Icons::EndLineDiamond,  tr("Carré",               "type of the 2nd end of a line"), Qet::Diamond );
	
	end1_length = new QDoubleSpinBox();
	end2_length = new QDoubleSpinBox();
	
	QGridLayout *grid = new QGridLayout();
	grid -> addWidget(new QLabel("x1"),        0, 0);
	grid -> addWidget(x1,                      0, 1);
	grid -> addWidget(new QLabel("y1"),        0, 2);
	grid -> addWidget(y1,                      0, 3);
	grid -> addWidget(new QLabel("x2"),        1, 0);
	grid -> addWidget(x2,                      1, 1);
	grid -> addWidget(new QLabel("y2"),        1, 2);
	grid -> addWidget(y2,                      1, 3);
	
	QGridLayout *grid2 = new QGridLayout();
	grid2 -> addWidget(new QLabel(tr("Fin 1")), 0, 0);
	grid2 -> addWidget(end1_type,               0, 1);
	grid2 -> addWidget(end1_length,             0, 2);
	grid2 -> addWidget(new QLabel(tr("Fin 2")), 1, 0);
	grid2 -> addWidget(end2_type,               1, 1);
	grid2 -> addWidget(end2_length,             1, 2);
	
	QVBoxLayout *v_layout = new QVBoxLayout(this);
	v_layout -> addWidget(style_);
	v_layout -> addLayout(grid);
	v_layout -> addLayout(grid2);
	v_layout->addStretch();
	updateForm();
}

/// Destructeur
LineEditor::~LineEditor() {
}

void LineEditor::setUpChangeConnections()
{
    m_change_connections << connect(part, &PartLine::lineChanged, this, &LineEditor::updateForm);
    m_change_connections << connect(part, &PartLine::firstEndTypeChanged, this, &LineEditor::updateForm);
    m_change_connections << connect(part, &PartLine::secondEndTypeChanged, this, &LineEditor::updateForm);
    m_change_connections << connect(part, &PartLine::firstEndLengthChanged, this, &LineEditor::updateForm);
    m_change_connections << connect(part, &PartLine::secondEndLengthChanged, this, &LineEditor::updateForm);
}

void LineEditor::disconnectChangeConnections()
{
    for (QMetaObject::Connection c : m_change_connections) {
        disconnect(c);
    }
    m_change_connections.clear();
}

/**
	@brief LineEditor::setPart
	Specifie to this editor the part to edit.
	Note that an editor can accept or refuse to edit a part. This editor accept only PartLine.
	@param new_part
	@return
*/
bool LineEditor::setPart(CustomElementPart *new_part)
{
	if (!new_part)
	{
		if (part)
		{
			disconnectChangeConnections();
		}
		part = nullptr;
		return(true);
	}
	if (PartLine *part_line = dynamic_cast<PartLine *>(new_part))
	{
		if (part == part_line) return true;
		if (part)
		{
			disconnectChangeConnections();
		}
		part = part_line;
		updateForm();
		setUpChangeConnections();
		return(true);
	}
	return(false);
}

bool LineEditor::setParts(QList <CustomElementPart *> parts)
{
	if (parts.isEmpty())
		return false;
	
	if (!setPart(parts.first()))
		return false;
	return style_->setParts(parts);
}

/**
	@return la primitive actuellement editee, ou 0 si ce widget n'en edite pas
*/
CustomElementPart *LineEditor::currentPart() const {
	return(part);
}

QList<CustomElementPart*> LineEditor::currentParts() const {
    return style_->currentParts();
}

/**
	@brief LineEditor::editedP1
	@return The edited P1 in item coordinate
*/
QPointF LineEditor::editedP1() const {
	return part -> mapFromScene(x1->value(), y1->value());
}

/**
	@brief LineEditor::editedP2
	@return The edited P2 in item coordinate
*/
QPointF LineEditor::editedP2() const {
	return part -> mapFromScene(x2->value(), y2->value());
}

/// Met a jour le type de la premiere extremite
void LineEditor::updateLineEndType1()
{
	if (m_locked) return;
	m_locked = true;
	QVariant end = end1_type -> itemData(end1_type->currentIndex());
	
	for (auto part: style_->currentParts()) {
		
		PartLine* line = static_cast<PartLine*>(part);
		
		if (end != line->property("end1"))
		{
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(line, "end1", line->property("end1"), end);
			undo->setText(tr("Modifier une ligne"));
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

/// Met a jour la longueur de la premiere extremite
void LineEditor::updateLineEndLength1()
{
	if (m_locked) return;
	m_locked = true;
	double length = end1_length->value();
	
	for (auto part: style_->currentParts()) {
		
		PartLine* line = static_cast<PartLine*>(part);
		
		if (length != line->property("length1"))
		{
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(line, "length1", line->property("length1"), length);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

/// Met a jour le type de la seconde extremite
void LineEditor::updateLineEndType2()
{
	if (m_locked) return;
	m_locked = true;
	QVariant end = end2_type -> itemData(end2_type->currentIndex());
	
	for (auto part: style_->currentParts()) {
		
		PartLine* line = static_cast<PartLine*>(part);
		
		if (end != line->property("end2"))
		{
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(line, "end2", line->property("end2"), end);
			undo->setText(tr("Modifier une ligne"));
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

/// Met a jour la longueur de la seconde extremite
void LineEditor::updateLineEndLength2()
{
	if (m_locked) return;
	m_locked = true;
	double length = end2_length->value();
	
	for (auto part: style_->currentParts()) {
		
		PartLine* line = static_cast<PartLine*>(part);
		
		if (length != line->property("length2"))
		{
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(line, "length2", line->property("length2"), length);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

void LineEditor::lineEditingFinishedX1()
{
	if (m_locked) return;
	m_locked = true;
	
	for (auto part: style_->currentParts()) {
		
		PartLine* l = static_cast<PartLine*>(part);
		QLineF line = l->property("line").toLineF();
		
		QPointF p1 = l->mapFromScene(x1->value(), y1->value());
		
		if (p1.x() != line.p1().x())
		{
			p1.setY(line.p1().y()); // restore old y value
			line.setP1(p1);
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(l, "line", l->property("line"), line);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

void LineEditor::lineEditingFinishedY1()
{
	if (m_locked) return;
	m_locked = true;
	
	for (auto part: style_->currentParts()) {
		
		PartLine* l = static_cast<PartLine*>(part);
		QLineF line = l->property("line").toLineF();
		
		QPointF p1 = l->mapFromScene(x1->value(), y1->value());
		
		if (p1.y() != line.p1().y())
		{
			p1.setX(line.p1().x()); // restore old x value
			line.setP1(p1);
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(l, "line", l->property("line"), line);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

void LineEditor::lineEditingFinishedX2()
{
	if (m_locked) return;
	m_locked = true;
	
	for (auto part: style_->currentParts()) {
		
		PartLine* l = static_cast<PartLine*>(part);
		QLineF line = l->property("line").toLineF();
		
		QPointF p2 = l->mapFromScene(x2->value(), y2->value());
		
		if (p2.x() != line.p1().x())
		{
			p2.setY(line.p2().y()); // restore old y value
			line.setP2(p2);
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(l, "line", l->property("line"), line);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

void LineEditor::lineEditingFinishedY2()
{
	if (m_locked) return;
	m_locked = true;
	
	for (auto part: style_->currentParts()) {
		
		PartLine* l = static_cast<PartLine*>(part);
		QLineF line = l->property("line").toLineF();
		
		QPointF p2 = l->mapFromScene(x2->value(), y2->value());
		
		if (p2.y() != line.p1().y())
		{
			p2.setX(line.p2().x()); // restore old y value
			line.setP2(p2);
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(l, "line", l->property("line"), line);
			undo->setText(tr("Modifier une ligne"));
			undo->enableAnimation();
			elementScene()->undoStack().push(undo);
		}
	}
	m_locked = false;
}

/**
	@brief LineEditor::updateForm
	Update the value of the widgets
*/
void LineEditor::updateForm()
{
	if (!part) return;
	activeConnections(false);
	QPointF p1(part -> sceneP1());
	QPointF p2(part -> sceneP2());
	x1 -> setValue(p1.x());
	y1 -> setValue(p1.y());
	x2 -> setValue(p2.x());
	y2 -> setValue(p2.y());
	end1_type -> setCurrentIndex(end1_type->findData(part -> firstEndType()));
	end1_length -> setValue(part -> firstEndLength());
	end2_type -> setCurrentIndex(end2_type->findData(part -> secondEndType()));
	end2_length -> setValue(part -> secondEndLength());
	activeConnections(true);
}

/**
	@brief LineEditor::activeConnections
	Enable/disable connection between editor widget and slot editingFinished
	True == enable | false == disable
	@param active
*/
void LineEditor::activeConnections(bool active)
{
	if (active)
	{
		connect(x1, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedX1()));
		connect(y1, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedY1()));
		connect(x2, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedX2()));
		connect(y2, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedY2()));
		connect(end1_type,   SIGNAL(currentIndexChanged(int)), this, SLOT(updateLineEndType1()));
		connect(end1_length, SIGNAL(editingFinished()),        this, SLOT(updateLineEndLength1()));
		connect(end2_type,   SIGNAL(currentIndexChanged(int)), this, SLOT(updateLineEndType2()));
		connect(end2_length, SIGNAL(editingFinished()),        this, SLOT(updateLineEndLength2()));
	}
	else
	{
		disconnect(x1, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedX1()));
		disconnect(y1, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedY1()));
		disconnect(x2, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedX2()));
		disconnect(y2, SIGNAL(editingFinished()), this, SLOT(lineEditingFinishedY2()));
		disconnect(end1_type,   SIGNAL(currentIndexChanged(int)), this, SLOT(updateLineEndType1()));
		disconnect(end1_length, SIGNAL(editingFinished()),        this, SLOT(updateLineEndLength1()));
		disconnect(end2_type,   SIGNAL(currentIndexChanged(int)), this, SLOT(updateLineEndType2()));
		disconnect(end2_length, SIGNAL(editingFinished()),        this, SLOT(updateLineEndLength2()));
	}
}

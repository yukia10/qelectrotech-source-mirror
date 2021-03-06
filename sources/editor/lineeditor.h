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
#ifndef LINE_EDITOR_H
#define LINE_EDITOR_H

#include "elementitemeditor.h"

class PartLine;
class StyleEditor;
class QComboBox;
class QDoubleSpinBox;

/**
	This class provides a widget to edit lines within the element editor.
*/
class LineEditor : public ElementItemEditor
{
	Q_OBJECT
		// constructors, destructor
	public:
		LineEditor(QETElementEditor *, PartLine * = nullptr, QWidget * = nullptr);
		~LineEditor() override;
	private:
		LineEditor(const LineEditor &);
	
		// attributes
	private:
		PartLine *part;
		StyleEditor *style_;
		QDoubleSpinBox *x1, *y1, *x2, *y2;
		QComboBox *end1_type, *end2_type;
		QDoubleSpinBox*end1_length, *end2_length;
		bool m_locked;
		QList <QMetaObject::Connection> m_change_connections;
	
		// methods
	public:
		bool setPart(CustomElementPart *) override;
		bool setParts(QList <CustomElementPart *> parts) override;
		CustomElementPart *currentPart() const override;
		QList<CustomElementPart*> currentParts() const override;
		QPointF editedP1() const;
		QPointF editedP2() const;
	
	public slots:
		void updateLineEndType1();
		void updateLineEndLength1();
		void updateLineEndType2();
		void updateLineEndLength2();
		void lineEditingFinishedX1();
		void lineEditingFinishedY1();
		void lineEditingFinishedX2();
		void lineEditingFinishedY2();
		void updateForm() override;
		
		private:
		void activeConnections(bool);
		/**
			@brief setUpChangeConnections
			Setup the connection from the line(s) to the widget,
			to update it when the line(s) are changed (moved ...)
		*/
		void setUpChangeConnections();
		void disconnectChangeConnections();
};
#endif

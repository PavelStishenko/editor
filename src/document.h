// SPDX-License-Identifier: BSL-1.0

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <functional>

#include <QIODevice>
#include <QObject>
#include <QString>
#include <QVector>

#include <Tui/ZTerminal.h>
#include <Tui/ZTextLayout.h>

class Document;

// TMP from tui widgets

template <typename Tag>
struct ListTrait;

template <typename T>
class ListNode;

template <typename T, typename Tag>
class ListHead {
public:
    ListHead(){}

    ~ListHead() {
        clear();
    }

    void clear() {
        while (first) {
            remove(first);
        }
    }

    void appendOrMoveToLast(T *e) {
        constexpr auto nodeOffset = ListTrait<Tag>::offset;
        auto& node = e->*nodeOffset;
        if (last == e) {
            return;
        }
        if (node.next) {
            //move
            remove(e);
        }
        if (last) {
            (last->*nodeOffset).next = e;
            node.prev = last;
            last = e;
        } else {
            first = e;
            last = e;
        }
    }

    void remove(T *e) {
        constexpr auto nodeOffset = ListTrait<Tag>::offset;
        auto &node = e->*nodeOffset;
        if (e == first) {
            first = node.next;
        }
        if (e == last) {
            last = node.prev;
        }
        if (node.prev) {
            (node.prev->*nodeOffset).next = node.next;
        }
        if (node.next) {
            (node.next->*nodeOffset).prev = node.prev;
        }
        node.prev = nullptr;
        node.next = nullptr;
    }

    T *first = nullptr;
    T *last = nullptr;
};


template <typename T>
class ListNode {
public:
    T *prev = nullptr;
    T *next = nullptr;
};

// /TMP

class RangeIterator {
public:
    int i;
    int operator *(){ return i; };
    bool operator !=(const RangeIterator & b) {return i!=b.i; }
    int operator ++() { return i++; }
};

class Range {
public:
    int start;
    int stop;
    RangeIterator begin() { return RangeIterator {start};}
    RangeIterator end() { return RangeIterator {stop};}
};

struct TextCursorToDocumentTag;

class TextCursor {
public:
    struct Position {
        int codeUnit = 0; // in line
        int line = 0;

        Position(int codeUnit, int line) : codeUnit(codeUnit), line(line) {}
        friend bool operator<(const Position& a, const Position& b);
        friend bool operator>(const Position& a, const Position& b);
        friend bool operator==(const Position& a, const Position& b);
    };


public:
    TextCursor(Document *doc, Tui::ZWidget *widget, std::function<Tui::ZTextLayout(int line, bool wrappingAllowed)> createTextLayout);
    ~TextCursor();

    void insertText(const QString &text);
    void removeSelectedText();
    void clearSelection();
    QString selectedText() const;

    void deleteCharacter();
    void deletePreviousCharacter();
    void deleteWord();
    void deletePreviousWord();
    void deleteLine();

    void moveCharacterLeft(bool extendSelection = false);
    void moveCharacterRight(bool extendSelection = false);
    void moveWordLeft(bool extendSelection = false);
    void moveWordRight(bool extendSelection = false);

    void moveUp(bool extendSelection = false);
    void moveDown(bool extendSelection = false);

    void moveToStartOfLine(bool extendSelection = false);
    void moveToStartIndentedText(bool extendSelection = false);
    void moveToEndOfLine(bool extendSelection = false);

    void moveToStartOfDocument(bool extendSelection = false);
    void moveToEndOfDocument(bool extendSelection = false);

    Position position();
    void setPosition(Position pos, bool extendSelection = false);
    void setPositionPreservingVerticalMovementColumn(Position pos, bool extendSelection = false);

    Position anchor();
    void setAnchorPosition(Position pos);

    int verticalMovementColumn();
    void setVerticalMovementColumn(int column);

    // for hasSelection() == true
    Position selectionStartPos() const;
    Position selectionEndPos() const;

    void selectAll();

    bool hasSelection() const;

    bool atStart() const;
    bool atEnd() const;
    bool atLineStart() const;
    bool atLineEnd() const;

public: // temporary hacks
    void tmp_ensureInRange();

private:
    //TextLayout createTextLayout(int line, bool wrappingAllowed);
    void updateVerticalMovementColumn(const Tui::ZTextLayout &layoutForCursorLine);

private:
    int _cursorCodeUnit = 0;
    int _cursorLine = 0;
    int _anchorCodeUnit = 0;
    int _anchorLine = 0;
    int _VerticalMovementColumn = 0;

    Document *_doc;
    Tui::ZWidget *_widget;
    std::function<Tui::ZTextLayout(int line, bool wrappingAllowed)> _createTextLayout;

private: // For use by Document
    ListNode<TextCursor> markersList;

    friend struct ListTrait<TextCursorToDocumentTag>;
    friend class Document;
};

template<>
struct ListTrait<TextCursorToDocumentTag> {
    static constexpr auto offset = &TextCursor::markersList;
};


struct LineMarkerToDocumentTag;

class LineMarker {
public:
    explicit LineMarker(Document *doc);
    explicit LineMarker(Document *doc, int line);
    ~LineMarker();

public:
    int line();
    void setLine(int line);

private:
    int _line = 0;
    Document *_doc = nullptr;

private: // For use by Document
    ListNode<LineMarker> markersList;

    friend struct ListTrait<LineMarkerToDocumentTag>;
    friend class Document;
};

template<>
struct ListTrait<LineMarkerToDocumentTag> {
    static constexpr auto offset = &LineMarker::markersList;
};


class Document : public QObject {
    Q_OBJECT
    friend class TextCursor;
    friend class LineMarker;

public:
    class UndoGroup;

public:
    Document(QObject *parent=nullptr);

public:
    void reset();
    void writeTo(QIODevice *file, bool crLfMode = false);
    bool readFrom(QIODevice *file);

    int lineCount() const;
    QString line(int line) const;
    int lineCodeUnits(int line) const;
    QVector<QString> getLines() const;

    bool isModified() const;

    void setNoNewline(bool value);
    bool noNewLine() const;

    QString filename() const;
    void setFilename(const QString &filename);

    void clearCollapseUndoStep();

    void tmp_sortLines(int first, int last, TextCursor *cursorForUndoStep);
    void tmp_moveLine(int from, int to, TextCursor *cursorForUndoStep);

    void undo(TextCursor *cursor);
    void redo(TextCursor *cursor);
    UndoGroup startUndoGroup(TextCursor *cursor);

    void initalUndoStep(TextCursor *cursor);

signals:
    void modificationChanged(bool changed);
    void redoAvailable(bool available);
    void undoAvailable(bool available);

public:
    class UndoGroup {
    public:
        UndoGroup(const UndoGroup&) = delete;
        UndoGroup &operator=(const UndoGroup&) = delete;

        ~UndoGroup();

    public:
        void closeGroup();

    private:
        friend class Document;
        UndoGroup(Document *doc, TextCursor *cursor);

        Document *_doc = nullptr;
        TextCursor *_cursor = nullptr;
        bool _closed = false;
    };

private: // TextCursor interface
    void removeFromLine(TextCursor *cursor, int line, int codeUnitStart, int codeUnits);
    void insertIntoLine(TextCursor *cursor, int line, int codeUnitStart, const QString &data);
    void appendToLine(TextCursor *cursor, int line, const QString &data);
    void removeLines(TextCursor *cursor, int start, int count);
    void insertLine(TextCursor *cursor, int before, const QString &data);
    void splitLine(TextCursor *cursor, TextCursor::Position pos);
    void saveUndoStep(TextCursor *cursor, bool collapsable=false);
    void registerTextCursor(TextCursor *marker);
    void unregisterTextCursor(TextCursor *marker);

private: // UndoGroup interface
    void closeUndoGroup(TextCursor *cursor);

private: // LineMarker interface
    void registerLineMarker(LineMarker *marker);
    void unregisterLineMarker(LineMarker *marker);

private:
    void emitModifedSignals();

private:
    struct UndoStep {
        QVector<QString> text;
        int cursorPositionX;
        int cursorPositionY;
    };

private:
    QString _filename;
    QVector<QString> _lines;
    bool _nonewline = false;

    QVector<UndoStep> _undoSteps;
    int _currentUndoStep = -1;
    int _savedUndoStep = -1;

    bool _collapseUndoStep = false;
    int _groupUndo = 0;
    bool _undoStepCreationDeferred = false;

    ListHead<LineMarker, LineMarkerToDocumentTag> lineMarkerList;
    ListHead<TextCursor, TextCursorToDocumentTag> cursorList;
};

#endif // DOCUMENT_H

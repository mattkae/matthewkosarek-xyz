var $ = window.$,
    rows = 3,
    columns = 3,
    currentLhs = [], // rows x columns matrix
    currentRhs = []; // rows x 1 matrix
    solutionContainer = $('#solution_container');

function main() {
    $('#reset_button').on('click', onResetClicked);
    $('#solve_button').on('click', onSolveClicked);
    resetView();
}

function resetView() {
    renderLHS();
    renderUnknowns();
    renderRHS();

    currentLhs = [];
    currentRhs = [];
    for (var r = 0; r < rows; r++) {
        for (var c = 0; c < columns; c++) {
            currentLhs.push(0);
        }
    }

    for (var otherR = 0; otherR < rows; otherR++) {
        currentRhs.push(0);
    }

    solutionContainer.empty();
}

// Renders for the input container.
function renderLHS() {
    var lhs = $('#matrix_lhs').empty(),
        addCell = function(index) {
            $('<input>').type('number').val(0).on('change',  function(event) {
                currentLhs[index] = Number(event.target.value);
            }).appendTo(lhs);
        };

    for (var r = 0; r < rows; r++) {
        for (var c = 0; c < columns; c++) {
            addCell((r * rows) + c);
        }
    }
}

function renderUnknowns() {
    var unknownsContainer = $('#unknown_variables').empty();

    for (var r = 0; r < columns; r++) {
        var cell = $('<input>').disabled(true).val(String.fromCharCode('x'.charCodeAt(0) + r).toUpperCase());
        unknownsContainer.append(cell);
    }
}

function renderRHS() {
    var rhs = $('#matrix_rhs').empty(),
        addCell = function(index) {
            $('<input>').type('number').val(0).on('change', function(event) {
                currentRhs[index] = Number(event.target.value);
            }).appendTo(rhs);
        };

    for (var r = 0; r < rows; r++) {
        addCell(r);
    }
}

// Renders for the augmented matrix container
function renderAugmentedMatrix(lhs, rhs) {
    var container = $('<div>').addClass('augmented_matrix_container'),
        lhsContainer = $('<div>').appendTo(container),
        rhsContainer = $('<div>').appendTo(container),
        addLhsCell = function(index) {
            $('<input>').type('number').val(lhs[index]).disabled(true).appendTo(lhsContainer);
        },
        addRhsCell = function(index) {
            $('<input>').type('number').val(rhs[index]).disabled(true).appendTo(rhsContainer);
        };
    
    for (var r = 0; r < rows; r++) {
        for (var c = 0; c < columns; c++) {
            addLhsCell(r * rows + c);
        }
    }

    for (var otherR = 0; otherR < rows; otherR++) {
        addRhsCell(otherR);
    }

    return container;
}

// Solver helper functions
function findLargestValue(c, rowStart) {
    var largestValue = 0,
        largestRow = -1;
    for (var r = rowStart; r < rows; r++) {
        var currValue = Math.abs(currentLhs[r * rows + c]);
        if (currValue > largestValue) {
            largestValue = currValue;
            largestRow = r;
        }
    }
    return {
        row: largestRow,
        value: largestValue
    };
}

function swapRows(r1, r2) {
    if (r1 === r2) {
        return;
    }

    for (var c = 0; c < columns; c++) {
        var temp = currentLhs[r1 * row + c];
        currentLhs[r1 * rows + c] = currentLhs[r2 * row + c];
        currentLhs[r2 * rows + c] = temp;
    }

    var rhsTmp = currentRhs[r1];
    currentRhs[r1] = currentRhs[r2];
    currentRhs[r2] = rhsTmp;
}

function multiplyRow(r, scale) {
    for (var c = 0; c < columns; c++) {
      currentLhs[r * rows + c] *= scale;
    }

    currentRhs[r] *= scale;
}

function addRow(r, a) {
    for (var c = 0; c < columns; c++) {
      currentLhs[r * rows + c] += a;
    }

    currentRhs[r] += a;
}

// Callbacks for actions
function onResetClicked() {
    resetView();
}

function onSolveClicked() {
    var rowStart = 0;
    for (var c = 0; c < columns; c++) {
        var largestForColumn = findLargestValue(c, rowStart);
        if (largestForColumn.row >= 0) {
            swapRows(rowStart, largestForColumn.row);
            multiplyRow(rowStart, 1.0 / largestForColumn.value);

            for (var r = 0; r < rows; r++) {
                if (r === rowStart) continue;

            }
        }

        rowStart++;
    }

    solutionContainer.append(renderAugmentedMatrix(currentLhs, currentRhs));
    solutionContainer.append(renderAugmentedMatrix(currentLhs, currentRhs));
}

main();

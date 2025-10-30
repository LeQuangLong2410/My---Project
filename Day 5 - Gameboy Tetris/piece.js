function orientPoints(pieceType, rotation){
    let results = [];
    switch (pieceType) {
        case 0: // I-piece
            switch (rotation) {
                case 0:
                    results = [
                        [-2, 0], [-1, 0], [0, 0], [1, 0]
                    ]; break;
                case 1:
                    results = [
                        [0, -1], [0, 0], [0, 1], [0, 2]
                    ]; break;
                case 2:
                    results = [
                        [-2, 1], [-1, 1], [0, 1], [1, 1]
                    ]; break;
                case 3:
                    results = [
                        [-1, -1], [-1, 0], [-1, 1], [-1, 2]
                    ]; break;
            }
            break;
        case 1: // J-piece
            switch (rotation) {
                case 0:
                    results = [
                        [-2, -1], [-2, 0], [-1, 0], [0, 0]
                    ]; break;
                case 1:
                    results = [
                        [-1, -1], [-1, 0], [-1, 1], [0, -1]
                    ]; break;
                case 2:
                    results = [
                        [-2, 0], [-1, 0], [0, 0], [0, 1]
                    ]; break;
                case 3:
                    results = [
                        [-1, -1], [-1, 0], [-1, 1], [-2, 1]
                    ]; break;
            }
            break;
        case 2: // L-piece
            switch (rotation) {
                case 0:
                    results = [
                        [-2, 0], [-1, 0], [0, 0], [0, -1]
                    ]; break;
                case 1:
                    results = [
                        [-1, -1], [-1, 0], [-1, 1], [0, 1]
                    ]; break;
                case 2:
                    results = [
                        [-2, 1], [-1, 1], [0, 1], [0, 0]
                    ]; break;
                case 3:
                    results = [
                        [-1, -1], [-1, 0], [-1, 1], [-2, -1]
                    ]; break;
            }
            break;
        case 3: // O-piece
            results = [
                [0, 0], [0, 1], [1, 0], [1, 1]
            ];
            break;
        case 4: // S-piece
            switch (rotation) {
                case 0:
                case 2:
                    results = [
                        [-1, 0], [0, 0], [0, 1], [1, 1]
                    ]; break;
                case 1:
                case 3:
                    results = [
                        [0, -1], [0, 0], [1, 0], [1, 1]
                    ]; break;
            }
            break;
        case 5: // T-piece
            switch (rotation) {
                case 0:
                    results = [
                        [-1, 0], [0, 0], [1, 0], [0, 1]
                    ]; break;
                case 1:
                    results = [
                        [0, -1], [0, 0], [0, 1], [1, 0]
                    ]; break;
                case 2:
                    results = [
                        [-1, 0], [0, 0], [1, 0], [0, -1]
                    ]; break;
                case 3:
                    results = [
                        [0, -1], [0, 0], [0, 1], [-1, 0]
                    ]; break;
            }
            break;
        case 6: // Z-piece
            switch (rotation) {
                case 0:
                case 2:
                    results = [
                        [-1, 1], [0, 1], [0, 0], [1, 0]
                    ]; break;
                case 1:
                case 3:
                    results = [
                        [1, -1], [1, 0], [0, 0], [0, 1]
                    ]; break;
            }
            break;
    }
    return results;
}
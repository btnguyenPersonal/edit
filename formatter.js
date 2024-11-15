module.exports = (results, data) => {
    var results = results || [];

    var summary = results.reduce(
        (seq, current) => {
            current.messages.forEach((msg) => {
                if (msg.severity === 2) {
                    seq.errors.push({
                        filePath: current.filePath,
                        message: msg.message,
                        line: msg.line,
                        column: msg.column
                    });
                }
            });
            return seq;
        },
        { errors: [] }
    );

    if (summary.errors.length > 0) {
        return summary.errors.map((msg) => (
            msg.filePath + ":" + msg.line + ":" + msg.column + ": Error: " + msg.message
        )).join("\n");
    }
};
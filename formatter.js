// eslint `state->buildDir` -f ~/formatter.js
module.exports = (results, data) => {
    var results = results || [];

    var summary = results.reduce(
        (seq, current) => {
            current.messages.forEach((msg) => {
                if (msg.severity === 2) {
                    seq.errors.push({
                        filePath: current.filePath,
                        ruleId: msg.ruleId,
                        ruleUrl: data.rulesMeta[msg.ruleId].docs.url,
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
            msg.filePath + ":" + msg.line + ":" + msg.column + " error: " + msg.message
        )).join("\n");
    }
};